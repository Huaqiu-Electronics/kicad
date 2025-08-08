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

#include <memory>
#include <stdexcept>
#include <string>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/webview.h>
#include <webview/webview_container.h>
#include <nlohmann/json.hpp>
#include <settings/webview_settings_manager.h>
#include <utils/generate_uuid.h>
#include <webview/webview_token_controller.h>
#include <webview/webview_script_msg_handler.h>
#include <webview/webview_functions.h>
#include <webview/webview_container.h>
#include <magic_enum.hpp>
#include <active_action/cmd/common/selection_change_cmd.h>
#include <utils/extract_host_from_url.h>
#include <utils/wstring_to_string.h>
#include "webview_container_impl.h"
#include <common/dialog_image_viewer.h>
#include <common/main_stack_holder.h>
#include <fmt/format.h>
#include <utils/send_data_buried_point.h>

#ifndef _WIN32
#include "webview_evt_filter.h"
#endif


using json = nlohmann::json;


enum START_UP_SIZE
{
    WIDTH = 400,
    HEIGHT = 1040
};


WEBVIEW_CONTAINER::WEBVIEW_CONTAINER( wxWindow* parent, std::optional<std::string> url,
                                      HOST_COPILOT_HANDLES       host_copilot_handles,
                                      std::optional<std::string> data_buried_point ) :
        wxPanel( parent ), _impl( std::make_unique<WEBVIEW_CONTAINER_IMPL>() )

{
    _main_stack_holder = ([=] () mutable -> MAIN_STACK_HOLDER* {

        MAIN_STACK_HOLDER* holder {};
        wxWindow* parent  = GetParent();

        while (!holder){

            holder = dynamic_cast<MAIN_STACK_HOLDER*>(parent);

            if( holder )
                return holder;

            if(parent)
                parent = parent->GetParent();

            if(!parent)
                break;
        }        

        throw new std::runtime_error( "main stack holder not found" ) ;

    })();


    _impl->_host_copilot_handles = std::move( host_copilot_handles );
    _impl->_primary_url = std::move( url );
    _impl->_browser = wxWebView::New();


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
    //Set a more sensible size for web browsing
    SetSize( FromDIP( wxSize( START_UP_SIZE::WIDTH, START_UP_SIZE::HEIGHT ) ) );

    // Connect the webview events
    Bind( wxEVT_WEBVIEW_NAVIGATING, &WEBVIEW_CONTAINER::OnNavigationRequest, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_NAVIGATED, &WEBVIEW_CONTAINER::OnNavigationComplete, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_LOADED, &WEBVIEW_CONTAINER::OnDocumentLoaded, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_ERROR, &WEBVIEW_CONTAINER::OnError, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_NEWWINDOW, &WEBVIEW_CONTAINER::OnNewWindow, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_TITLE_CHANGED, &WEBVIEW_CONTAINER::OnTitleChanged, this, _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_FULLSCREEN_CHANGED, &WEBVIEW_CONTAINER::OnFullScreenChanged, this, _impl->_browser->GetId() );

    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WEBVIEW_CONTAINER::OnScriptMessageWrapper, this,
          _impl->_browser->GetId() );
    Bind( wxEVT_WEBVIEW_SCRIPT_RESULT, &WEBVIEW_CONTAINER::OnScriptResult, this, _impl->_browser->GetId() );

    if( data_buried_point )
    {
        send_data_buried_point( *data_buried_point );
    }


#ifdef DEBUG

    _impl->_browser->EnableAccessToDevTools( true );

#endif // DEBUG


#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().AddPanel( this );
#endif

    WEBVIEW_TOKEN_CONTROLLER::Get().AddWebviewContainer( this );
}

WEBVIEW_CONTAINER::~WEBVIEW_CONTAINER()
{
#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().RemovePanel( this );
#endif

    WEBVIEW_TOKEN_CONTROLLER::Get().RemoveWebviewContainer( this );
}

void WEBVIEW_CONTAINER::ExecuteCommand( const char* cmd )
{
    const auto js_call =
            fmt::format( "{}( {} );", magic_enum::enum_name( WEBVIEW_FUNCTIONS::fire_host_active_cmd ), cmd );
    RunScriptAsync( js_call );
}


void WEBVIEW_CONTAINER::OnHostSelectionChanged( bool has_selection )
{
    ExecuteCommand( json( SELECTION_CHANGE_CMD{ has_selection } ).dump().c_str() );
}


void WEBVIEW_CONTAINER::OnNavigationRequest( wxWebViewEvent& evt )
{
    if( !_impl->_primary_url )
        return;

    const auto url = evt.GetURL();

    if( url == wxASCII_STR( wxWebViewDefaultURLStr ) )
    {
        evt.Veto();
        return;
    }

    if( !url.Contains( extract_host_from_url( *_impl->_primary_url ) ) )
    {
        // Launch the url in the system default _impl->_browser
        wxLaunchDefaultBrowser( url );
        evt.Veto();
        return;
    }
}


void WEBVIEW_CONTAINER::OnNavigationComplete( wxWebViewEvent& evt )
{
}

void WEBVIEW_CONTAINER::OnDocumentLoaded( wxWebViewEvent& evt )
{
    WEBVIEW_TOKEN_CONTROLLER::Get().OnPageLoaded( this );
}

void WEBVIEW_CONTAINER::OnNewWindow( wxWebViewEvent& evt )
{
    _impl->_browser->LoadURL( evt.GetURL() );
    evt.Veto(); // Prevent WebKit from creating a new page
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
    CallAfter(
            [&]
            {
                _main_stack_holder->RunMainStack(
                        [&]
                        {
                            if( evt.IsError() )
                                wxLogDebug( "Async JS failed: %s", evt.GetString() );
                            else
                                wxLogDebug( "Async JS succeeded: %s", evt.GetString() );
                        } );
            } );
}

void WEBVIEW_CONTAINER::OnError( wxWebViewEvent& evt )
{
#define WX_ERROR_CASE( type )                                                                                          \
    case type: category = #type; break;

    wxString category;
    switch( evt.GetInt() )
    {
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_CONNECTION );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_CERTIFICATE );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_AUTH );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_SECURITY );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_NOT_FOUND );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_REQUEST );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_USER_CANCELLED );
        WX_ERROR_CASE( wxWEBVIEW_NAV_ERR_OTHER );
    }

    wxLogDebug( "%s", "Error; url='" + evt.GetURL() + "', error='" + category + " (" + evt.GetString() + ")'" );
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
    CallAfter(
            [&]
            {
                _main_stack_holder->RunMainStack(
                        [this, aScript]()
                        {
                            const auto js_script =
                                    fmt::format( "try {{ {}; }} catch (e) {{console.error('JS exception:', e);}}",
                                                 aScript.ToStdString() );
                            wxString out;
                            _impl->_browser->RunScriptAsync( js_script, &out );
                            wxLogDebug( "ExecuteCommand (wrapped): %s", out );
                        } );
            } );
}


void WEBVIEW_CONTAINER::OnScriptMessageWrapper( wxWebViewEvent& evt )
{
    CallAfter(
            [&]
            {
                _main_stack_holder->RunMainStack(
                        [&]()
                        {
                            OnScriptMessage( evt );
                        } );
            } );
}