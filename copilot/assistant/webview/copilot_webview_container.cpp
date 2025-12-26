/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2025 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "webview/copilot_webview_container.h"
#include "copilot_webview_container_impl.h"
#include "host_version_info.h"

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <memory>
#include <utils/wstring_to_string.h>
#include <passive_action/passive_action_container.h>
#include <mcp/mcp_client_event.h>
#include <settings/webview_settings_manager.h>
#include <utils/httplib_wrapper.h>
#include <webview/webview_functions.h>
#include <wx/utils.h>
#include <kicad_copilot_editors.h>
#include <mcp/fmt_mcp_msg_script.h>
#include <utils/gen_nng_addr.h>

#if defined( IS_PORTABLE )

constexpr auto kDataBuriedPoint = "kicad_client_copilot_launch_portable";

#else

constexpr auto kDataBuriedPoint = "kicad_client_copilot_launch";

#endif

COPILOT_WEBVIEW_CONTAINER::COPILOT_WEBVIEW_CONTAINER( HOST_VERSION_INFO const& host_version_info, wxWindow* parent,
                                                      std::string url, HOST_COPILOT_HANDLES host_copilot_handles ) :
        WEBVIEW_CONTAINER( host_version_info, parent, url, host_copilot_handles, kDataBuriedPoint ),
        _impl( new COPILOT_WEBVIEW_CONTAINER_IMPL )
{
    if( WEBVIEW_SETTINGS_MANAGER::get_instance().mcp_enabled() )
    {
        auto sdk_listen_addr = gen_nng_addr();
        _impl->sdkServer = std::make_unique<KICAD_SDK_SERVER>( host_copilot_handles, this, sdk_listen_addr );
        _impl->mcpClientWorker = std::make_unique<MCP_CLIENT_WORKER>( gen_nng_addr(), *this,
                                                                      _impl->mcpClientRequestQueue, sdk_listen_addr );
        Bind( EVT_MCP_AGENT_MSG, &COPILOT_WEBVIEW_CONTAINER::OnMcpAgentMessage, this );
    }
}
COPILOT_WEBVIEW_CONTAINER::~COPILOT_WEBVIEW_CONTAINER()
{
    if( _impl->sdkServer )
        _impl->sdkServer->Stop();

    if( _impl->mcpClientWorker )
        _impl->mcpClientWorker->Stop();
}

void COPILOT_WEBVIEW_CONTAINER::OnDocumentLoaded( wxWebViewEvent& evt )
{
    WEBVIEW_CONTAINER::OnDocumentLoaded( evt );

#ifndef _WIN32

    const auto evt_url = evt.GetURL().ToStdString();

    // NOTE: Adhoc, on *nux the webview is not loading the url correctly on the first time if url start with https
    if( evt_url == WEBVIEW_SETTINGS_MANAGER::get_instance().get_copilot_webview_url() )
    {
        LoadUrl( *GetUrl() );
    }

#endif // !_WIN32
};

void COPILOT_WEBVIEW_CONTAINER::OnNavigationRequest( wxWebViewEvent& evt )
{
    const auto url = evt.GetURL();

    if( !url.Contains( WEBVIEW_SETTINGS_MANAGER::get_instance().get_copilot_webview_url() ) )
    {
        // Launch the url in the system default browser
        wxLaunchDefaultBrowser( url );
        evt.Veto();
        return;
    }
}

void COPILOT_WEBVIEW_CONTAINER::OnNewWindow( wxWebViewEvent& evt )
{
    LoadUrl( evt.GetURL().ToStdString() );
    evt.Veto(); // Prevent WebKit from creating a new page
}

void COPILOT_WEBVIEW_CONTAINER::HandleScriptMessage( std::string const& aMessage )
{
    try
    {
        const auto act_container = nlohmann::json::parse( aMessage ).get<PASSIVE_ACTION_CONTAINER>();
        switch( act_container.category )
        {
        case PA_MCP:
        {
            _impl->mcpClientRequestQueue.Post( act_container.data.dump() );

            break;
        }
        default: return WEBVIEW_CONTAINER::HandleScriptMessage( aMessage );
        }
    }
    catch( std::exception& e )
    {
        wxLogError( "Failed to parse MCP message: %s", e.what() );
    }
}

void COPILOT_WEBVIEW_CONTAINER::OnMcpAgentMessage( MCP_CLIENT_EVENT& aMessage )
{
    RunScriptAsync( fmt_mcp_msg_script( wxString2String( aMessage.GetString() ) ) );
}
