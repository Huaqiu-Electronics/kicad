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

#include <evt_dispatcher_holder.h>
#include <memory>
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
#include <fmt/format.h>
#include <utils/send_data_buried_point.h>


using json = nlohmann::json;


enum START_UP_SIZE
{
    WIDTH = 400,
    HEIGHT = 1040
};


WEBVIEW_CONTAINER::WEBVIEW_CONTAINER( wxWindow* parent, std::optional<std::string> url,
                                      HOST_COPILOT_HANDLES       host_copilot_handles,
                                      std::optional<std::string> data_buried_point ) :
        wxPanel( parent ),
        _impl( std::make_unique<WEBVIEW_CONTAINER_IMPL>() )

{
    _impl->_host_copilot_handles = std::move( host_copilot_handles );
    _impl->_primary_url = std::move( url );
    _impl->_browser = wxWebView::New();


#ifdef __WXMAC__
    _impl->_evt_dispatcher_holder= ([&] () -> EVT_DISPATCHER_HOLDER* {

        EVT_DISPATCHER_HOLDER* holder {};
        wxWindow* parent  = GetParent();

        while (!holder){

            holder = dynamic_cast<EVT_DISPATCHER_HOLDER*>(parent);

            if( holder )
                return holder;

            if(parent)
                parent = parent->GetParent();

            if(!parent)
                break;
        }        

        throw new std::runtime_error( "main stack holder not found" ) ;

    })();


    if( _impl->_evt_dispatcher_holder )
    {
        _impl->_evt_dispatcher_holder->AddPanel( _impl->_browser );
    }

#endif    

    auto top_sizer = new wxBoxSizer( wxVERTICAL );
    _impl->_browser->Create( this, wxID_ANY,
                             _impl->_primary_url ? *_impl->_primary_url : wxASCII_STR( wxWebViewDefaultURLStr ),
                             wxDefaultPosition, wxDefaultSize );
    top_sizer->Add( _impl->_browser, wxSizerFlags().Expand().Proportion( 1 ) );


    if( !_impl->_browser->AddScriptMessageHandler(
                magic_enum::enum_name( WEBVIEW_SCRIPT_MSG_HANDLER::eda_host ).data() ) )
    {
        wxLogDebug( "Could not add script message handler " );
    }

    SetSizer( top_sizer );
    SetSize( FromDIP( wxSize( START_UP_SIZE::WIDTH, START_UP_SIZE::HEIGHT ) ) );

    Bind( wxEVT_WEBVIEW_NAVIGATING, &WEBVIEW_CONTAINER::OnNavigationRequest, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_NAVIGATED, &WEBVIEW_CONTAINER::OnNavigationComplete, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_LOADED, &WEBVIEW_CONTAINER::OnDocumentLoaded, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_ERROR, &WEBVIEW_CONTAINER::OnError, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_NEWWINDOW, &WEBVIEW_CONTAINER::OnNewWindow, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_TITLE_CHANGED, &WEBVIEW_CONTAINER::OnTitleChanged, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_FULLSCREEN_CHANGED, &WEBVIEW_CONTAINER::OnFullScreenChanged, this, _impl->_browser->GetId() );

    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WEBVIEW_CONTAINER::OnScriptMessage, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_SCRIPT_RESULT, &WEBVIEW_CONTAINER::OnScriptResult, this, _impl->_browser->GetId() );

    if( data_buried_point )
    {
        send_data_buried_point( *data_buried_point );
    }

    WEBVIEW_CONTROLLER::Get().AddWebviewContainer( this );

#ifdef DEBUG

    _impl->_browser->EnableAccessToDevTools( true );

#endif // DEBUG
}

WEBVIEW_CONTAINER::~WEBVIEW_CONTAINER()
{
#ifdef __WXMAC__

    if( _impl->_evt_dispatcher_holder )
    {
        _impl->_evt_dispatcher_holder->RemovePanel( _impl->_browser );
    }

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
    WEBVIEW_CONTROLLER::Get().OnWebViewNavigationComplete( this );
}

void WEBVIEW_CONTAINER::OnDocumentLoaded( wxWebViewEvent& evt )
{
    wxLogDebug( "OnDocumentLoaded , %s", evt.GetURL() );
}

void WEBVIEW_CONTAINER::OnNewWindow( wxWebViewEvent& evt )
{
    wxLaunchDefaultBrowser( evt.GetURL() );
}

void WEBVIEW_CONTAINER::OnTitleChanged( wxWebViewEvent& evt )
{
}

void WEBVIEW_CONTAINER::OnFullScreenChanged( wxWebViewEvent& evt )
{
    if( auto p = dynamic_cast<wxDialog*>( GetParent() ) )
        p->ShowFullScreen( evt.GetInt() != 0 );

    evt.Veto();
}


void WEBVIEW_CONTAINER::OnScriptResult( wxWebViewEvent& evt )
{
    WXUNUSED( evt );
}

void WEBVIEW_CONTAINER::OnError( wxWebViewEvent& evt )
{
    WXUNUSED( evt );
}


std::optional<std::string> WEBVIEW_CONTAINER::GetUrl() const
{
    return _impl->_primary_url;
}


void WEBVIEW_CONTAINER::LoadUrl( std::string const& aUrl )
{
    _impl->_primary_url = aUrl;
    _impl->_browser->LoadURL( aUrl );
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
}