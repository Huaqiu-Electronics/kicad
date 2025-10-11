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


#include "webview_container_impl.h"
#include "internal/webview_evt_filter.h"
#include <optional>

#ifndef _WIN32
#include "internal/webview_evt_filter.h"
#endif

#include <memory>
#include <stdexcept>
#include <string>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/webview.h>
#include <nlohmann/json.hpp>
#include <settings/webview_settings_manager.h>
#include <utils/generate_uuid.h>
#include <webview/webview_script_msg_handler.h>
#include <webview/webview_functions.h>
#include <webview/webview_controller.h>
#include <webview/webview_container.h>
#include <magic_enum.hpp>
#include <active_action/common/selection_change_cmd.h>
#include <utils/extract_host_from_url.h>
#include <utils/wstring_to_string.h>
#include <common/dialog_image_viewer.h>
#include <common/main_stack_holder.h>
#include <fmt/format.h>
#include <utils/send_data_buried_point.h>
#include <utils/query_string_cache_busting.h>
#include <settings/copilot_config_version.h>

using json = nlohmann::json;


enum START_UP_SIZE
{
    WIDTH = 400,
    HEIGHT = 1040
};


inline auto ensure_load_page_from_srv( std::string const& url )
{
    return query_string_cache_busting( url, kConfigVersion );
}


WEBVIEW_CONTAINER::WEBVIEW_CONTAINER( HOST_VERSION_INFO const& host_version_info, wxWindow* parent,
                                      std::optional<std::string> url, HOST_COPILOT_HANDLES host_copilot_handles,
                                      std::optional<std::string> data_buried_point ) :
        wxPanel( parent ), _impl( std::make_unique<WEBVIEW_CONTAINER_IMPL>() )

{
    _impl->_host_copilot_handles = std::move( host_copilot_handles );
    _impl->_primary_url = std::move( url );
    _impl->_browser = wxWebView::New();
    _impl->host_version_info = host_version_info;


    auto top_sizer = new wxBoxSizer( wxVERTICAL );
    _impl->_browser->Create( this, wxID_ANY,
                             _impl->_primary_url ? ensure_load_page_from_srv( *_impl->_primary_url )
                                                 : wxASCII_STR( wxWebViewDefaultURLStr ),
                             wxDefaultPosition, wxDefaultSize );
    top_sizer->Add( _impl->_browser, wxSizerFlags().Expand().Proportion( 1 ) );


    if( !_impl->_browser->AddScriptMessageHandler(
                magic_enum::enum_name( WEBVIEW_SCRIPT_MSG_HANDLER::eda_host ).data() ) )
    {
        wxLogDebug( "Could not add script message handler " );
    }

    SetSizer( top_sizer );
    SetSize( FromDIP( wxSize( START_UP_SIZE::WIDTH, START_UP_SIZE::HEIGHT ) ) );


    const auto browser_id = _impl->_browser->GetId();
    Bind( wxEVT_WEBVIEW_NAVIGATING, &WEBVIEW_CONTAINER::OnNavigationRequest, this, browser_id );
    Bind( wxEVT_WEBVIEW_NAVIGATED, &WEBVIEW_CONTAINER::OnNavigationComplete, this, browser_id );
    Bind( wxEVT_WEBVIEW_LOADED, &WEBVIEW_CONTAINER::OnDocumentLoaded, this, browser_id );
    Bind( wxEVT_WEBVIEW_ERROR, &WEBVIEW_CONTAINER::OnError, this, browser_id );
    Bind( wxEVT_WEBVIEW_NEWWINDOW, &WEBVIEW_CONTAINER::OnNewWindow, this, browser_id );
    Bind( wxEVT_WEBVIEW_TITLE_CHANGED, &WEBVIEW_CONTAINER::OnTitleChanged, this, browser_id );
    Bind( wxEVT_WEBVIEW_FULLSCREEN_CHANGED, &WEBVIEW_CONTAINER::OnFullScreenChanged, this, browser_id );
    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WEBVIEW_CONTAINER::OnScriptMessage, this, browser_id );


    if( data_buried_point )
    {
        send_data_buried_point( *data_buried_point );
    }

    if( WEBVIEW_SETTINGS_MANAGER::get_instance().webview_dev_tool_enabled() )
    {
        _impl->_browser->EnableAccessToDevTools( true );
    }


#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().AddPanel( this );
#endif

    WEBVIEW_CONTROLLER::Get().AddWebviewContainer( this );
}

WEBVIEW_CONTAINER::~WEBVIEW_CONTAINER()
{
#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().RemovePanel( this );
#endif

    WEBVIEW_CONTROLLER::Get().RemoveWebviewContainer( this );
}

void WEBVIEW_CONTAINER::ExecuteCommand( const char* cmd )
{
    const auto js_call =
            fmt::format( "{}( {} );", magic_enum::enum_name( WEBVIEW_FUNCTIONS::fire_host_active_cmd ), cmd );
    RunScriptAsync( std::move( js_call ) );
}


void WEBVIEW_CONTAINER::OnHostSelectionChanged( bool has_selection )
{
    ExecuteCommand( json( SELECTION_CHANGE_CMD{ has_selection } ).dump().c_str() );
}


void WEBVIEW_CONTAINER::OnNavigationRequest( wxWebViewEvent& evt )
{
    WXUNUSED( evt );
}


void WEBVIEW_CONTAINER::OnNavigationComplete( wxWebViewEvent& evt )
{
    wxLogDebug( "OnNavigationComplete , %s", evt.GetURL() );
}

void WEBVIEW_CONTAINER::OnDocumentLoaded( wxWebViewEvent& evt )
{
    wxLogDebug( "OnDocumentLoaded , %s", evt.GetURL() );

    RunScriptAsync( fmt::format( "window.host_version_info={};", nlohmann::json( _impl->host_version_info ).dump() ) );
    WEBVIEW_CONTROLLER::Get().OnWebViewNavigationComplete( this );
}

void WEBVIEW_CONTAINER::OnNewWindow( wxWebViewEvent& evt )
{
    wxLaunchDefaultBrowser( evt.GetURL() );
}

void WEBVIEW_CONTAINER::OnTitleChanged( wxWebViewEvent& evt )
{
    evt.Veto();
}

void WEBVIEW_CONTAINER::OnFullScreenChanged( wxWebViewEvent& evt )
{
    if( auto p = dynamic_cast<wxDialog*>( GetParent() ) )
        p->ShowFullScreen( evt.GetInt() != 0 );

    evt.Veto();
}

void WEBVIEW_CONTAINER::OnError( wxWebViewEvent& evt )
{
    evt.Veto();
}


std::optional<std::string> WEBVIEW_CONTAINER::GetUrl() const
{
    return _impl->_primary_url;
}


void WEBVIEW_CONTAINER::LoadUrl( std::string const& aUrl )
{
    _impl->_primary_url = aUrl;
    _impl->_browser->LoadURL( ensure_load_page_from_srv( aUrl ) );
}


void WEBVIEW_CONTAINER::RunScriptAsync( wxString const& aScript )
{
    const auto js_script =
            fmt::format( "try {{ {}; }} catch (e) {{console.error('JS exception:', e);}}", aScript.ToStdString() );
    _impl->_browser->RunScriptAsync( std::move( js_script ) );
}


void WEBVIEW_CONTAINER::OnScriptMessage( wxWebViewEvent& evt )
{
    HandleScriptMessage( wxString2String( evt.GetString() ) );
    evt.Veto(); // Prevent default handling
}