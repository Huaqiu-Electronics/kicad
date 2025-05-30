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
#include <string>
#include <webview/webview_container.h>
#include "nlohmann/json.hpp"
#include "settings/copilot_settings.h"
#include "settings/copilot_settings_manager.h"
#include "web_utils.h"
#include "webview_constant.h"
#include <utils/copilot_utils.h>
#include <passive_action/web/web_host.h>
#include <passive_action/passive_action_container.h>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>
#include <active_action/cmd/common/selection_change_cmd.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/webview.h>
#include <wx/webviewarchivehandler.h>
#include <wx/webviewfshandler.h>
#include <context/copilot_global_context_handle.h>
#include <context/copilot_global_context.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <wx/timer.h>
#include <thread>
#include "http/http_client.h"
#include "proto/data_buried_point.h"
#include "common/dialog_image_viewer.h"
#include <utils/httplib.h>

#ifdef _WIN32
inline std::string ws2s( const std::wstring& wstr )
{
    if( wstr.empty() )
        return std::string();
    int size_needed =
            WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int) wstr.size(), NULL, 0, NULL, NULL );
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int) wstr.size(), &strTo[0], size_needed, NULL,
                         NULL );
    return strTo;
}

#endif // _WIN32


using json = nlohmann::json;
using namespace httplib;

inline auto send_data_buried_point()
{
    std::thread t(
            [&]()
            {
                try
                {
                    HTTP_CLIENT http_client;
                    http_client.send_data_buried_point( DATA_BURIED_POINT{} );
                }
                catch( std::exception const& e )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: %s", e.what() ),
                                "send_data_buried_point" );
                }
                catch( ... )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: unknown" ),
                                "send_data_buried_point" );
                }
            } );
    t.detach();
}


enum START_UP_SIZE
{
    WIDTH = 400,
    HEIGHT = 1040
};

WEBVIEW_CONTAINER::WEBVIEW_CONTAINER( wxWindow*            parent,
                                      HOST_COPILOT_HANDLES host_copilot_handles ) :
        wxPanel( parent ), _browser( wxWebView::New() ),
        _host_copilot_handles( std::move( host_copilot_handles ) ),
        _primary_url( add_parameter_to_url(
                COPILOT_SETTINGS_MANAGER::get_instance().get_webview_chat_path(),
                _host_copilot_handles.host_type ) )
{
#ifndef COPILOT_USE_SCRIPT_MSG_HANDLER

    _server = std::make_unique<httplib::Server>();
    _port = _server->bind_to_any_port( "0.0.0.0" );
    _host_copilot_handles.host_type->port = _port;
    _primary_url =
            add_parameter_to_url( COPILOT_SETTINGS_MANAGER::get_instance().get_webview_chat_path(),
                                  _host_copilot_handles.host_type );
    _server_thread = std::make_unique<std::thread>(
            [&]()
            {
                // Handle POST requests
                _server->Post( "/passive_action",
                               [this]( const Request& req, Response& res )
                               {
                                   // Add CORS headers
                                   res.set_header( "Access-Control-Allow-Origin", "*" );
                                   res.set_header( "Access-Control-Allow-Methods",
                                                   "POST, OPTIONS" );
                                   res.set_header( "Access-Control-Allow-Headers", "*" );

                                   const auto msg = req.body;
                                   if( msg.empty() )
                                   {
                                       res.set_content( "Invalid message", "text/plain" );
                                       return;
                                   }

                                   // Process the message
                                   generic_process_web_msg_wrapper( msg );
                                   res.set_content( "OK", "text/plain" );
                               } );

                // Handle OPTIONS preflight requests
                _server->Options( "/passive_action",
                                  []( const Request& req, Response& res )
                                  {
                                      res.set_header( "Access-Control-Allow-Origin", "*" );
                                      res.set_header( "Access-Control-Allow-Methods",
                                                      "POST, OPTIONS" );
                                      res.set_header( "Access-Control-Allow-Headers", "*" );
                                      res.set_content( "", "text/plain" );
                                  } );


                _server->listen_after_bind();
            } );

#endif // !COPILOT_USE_SCRIPT_MSG_HANDLER

    auto top_sizer = new wxBoxSizer( wxVERTICAL );

    // ADHOC : force client generate a new session
    if( _host_copilot_handles.host_type )
    {
        // Generate a uid
        _host_copilot_handles.host_type->session_id = generate_uuid();
    }

    _browser->Create( this, wxID_ANY, _primary_url, wxDefaultPosition, wxDefaultSize );
    top_sizer->Add( _browser, wxSizerFlags().Expand().Proportion( 1 ) );

#ifdef COPILOT_USE_SCRIPT_MSG_HANDLER

    // FIXME : wxWebView::AddScriptMessageHandler is not available on MacOS , it will lead to crash while working with the KiCad' co-thread

    if( !_browser->AddScriptMessageHandler(
                magic_enum::enum_name( WEBVIEW_MSG_HANDLES::eda_host ).data() ) )
    {
        wxLogDebug( "Could not add script message handler " );
    }


#endif // COPILOT_USE_SCRIPT_MSG_HANDLER


    SetSizer( top_sizer );
    //Set a more sensible size for web browsing
    SetSize( FromDIP( wxSize( START_UP_SIZE::WIDTH, START_UP_SIZE::HEIGHT ) ) );

    // Connect the webview events
    Bind( wxEVT_WEBVIEW_NAVIGATING, &WEBVIEW_CONTAINER::OnNavigationRequest, this,
          _browser->GetId() );
    Bind( wxEVT_WEBVIEW_NAVIGATED, &WEBVIEW_CONTAINER::OnNavigationComplete, this,
          _browser->GetId() );
    Bind( wxEVT_WEBVIEW_LOADED, &WEBVIEW_CONTAINER::OnDocumentLoaded, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_ERROR, &WEBVIEW_CONTAINER::OnError, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_NEWWINDOW, &WEBVIEW_CONTAINER::OnNewWindow, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_TITLE_CHANGED, &WEBVIEW_CONTAINER::OnTitleChanged, this,
          _browser->GetId() );
    Bind( wxEVT_WEBVIEW_FULLSCREEN_CHANGED, &WEBVIEW_CONTAINER::OnFullScreenChanged, this,
          _browser->GetId() );

#ifdef COPILOT_USE_SCRIPT_MSG_HANDLER

    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WEBVIEW_CONTAINER::OnScriptMessage, this,
          _browser->GetId() );
#endif
    Bind( wxEVT_WEBVIEW_SCRIPT_RESULT, &WEBVIEW_CONTAINER::OnScriptResult, this,
          _browser->GetId() );

#ifdef DEBUG

    _browser->EnableAccessToDevTools( true );

#endif // DEBUG

    send_data_buried_point();
}

WEBVIEW_CONTAINER::~WEBVIEW_CONTAINER()
{
#ifndef COPILOT_USE_SCRIPT_MSG_HANDLER

    _server->stop();
    _server_thread->join();

#endif // !COPILOT_USE_SCRIPT_MSG_HANDLER
}

void WEBVIEW_CONTAINER::fire_host_active_cmd( const char* cmd )
{
    const auto js_call =
            fmt::format( "try {{ {}({}); }} catch (e) {{ console.error('JS exception:', e); }}",
                         magic_enum::enum_name( WEBVIEW_FUNCTIONS::fire_host_active_cmd ), cmd );

    CallAfter(
            [this, js_call]()
            {
                wxString out;
                _browser->RunScriptAsync( js_call, &out );
                wxLogDebug( "fire_host_active_cmd (wrapped): %s", out );
            } );
}


void WEBVIEW_CONTAINER::on_host_selection_changed( bool has_selection )
{
    fire_host_active_cmd( json( SELECTION_CHANGE_CMD{ has_selection } ).dump().c_str() );
}


void WEBVIEW_CONTAINER::OnNavigationRequest( wxWebViewEvent& evt )
{
    const auto url = evt.GetURL();

    if( !url.Contains( COPILOT_SETTINGS_MANAGER::get_instance().get_webview_url() ) )
    {
        // Launch the url in the system default browser
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
#ifndef _WIN32

    const auto evt_url = evt.GetURL().ToStdString();

    // NOTE: Adhoc, on *nux the webview is not loading the url correctly on the first time if url start with https
    if( evt_url == COPILOT_SETTINGS_MANAGER::get_instance().get_webview_url() )
    {
        _browser->LoadURL( _primary_url );
    }

#endif // !_WIN32
}

void WEBVIEW_CONTAINER::OnNewWindow( wxWebViewEvent& evt )
{
    wxString flag = " (other)";

    if( evt.GetNavigationAction() == wxWEBVIEW_NAV_ACTION_USER )
    {
        flag = " (user)";
    }


    //If we handle new window events then just load them in this window as we
    //are a single window browser
    _browser->LoadURL( evt.GetURL() );
}

void WEBVIEW_CONTAINER::OnTitleChanged( wxWebViewEvent& evt )
{
}

void WEBVIEW_CONTAINER::OnFullScreenChanged( wxWebViewEvent& evt )
{
    // TODO
    // ShowFullScreen( evt.GetInt() != 0 );
}


#ifdef COPILOT_USE_SCRIPT_MSG_HANDLER

// NOTE : This is the entrypoint for HQ distribution v9.0.2 , which has been released on 2025-05
// The msg is posted through _browser->AddScriptMessageHandler( "eda_host" )
// Hover, it will lead to crash on MacOS , so the http way is preferred
void WEBVIEW_CONTAINER::OnScriptMessage( wxWebViewEvent& evt )
{
#ifdef _WIN32

    const auto wStr = evt.GetString().ToStdWstring();
    const auto str = ws2s( wStr );
    generic_process_web_msg( str );

#else

    generic_process_web_msg( evt.GetString().ToStdString() );

#endif // _WIN32
}

#endif

void WEBVIEW_CONTAINER::OnScriptResult( wxWebViewEvent& evt )
{
    if( evt.IsError() )
        wxLogDebug( "Async JS failed: %s", evt.GetString() );
    else
        wxLogDebug( "Async JS succeeded: %s", evt.GetString() );
}

void WEBVIEW_CONTAINER::OnError( wxWebViewEvent& evt )
{
#define WX_ERROR_CASE( type )                                                                      \
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

    wxLogDebug( "%s", "Error; url='" + evt.GetURL() + "', error='" + category + " ("
                              + evt.GetString() + ")'" );
}

void WEBVIEW_CONTAINER::generic_process_web_msg( std::string const& msg )
{
    try
    {
        const auto act_container = nlohmann::json::parse( msg ).get<PASSIVE_ACTION_CONTAINER>();
        switch( act_container.category )
        {
        case INVALID: throw std::runtime_error( "Invalid message received" );
        case PA_WEB_HOST:
        {
            const auto cmd = act_container.data.get<WEB_HOST_INTERNAL_CMD>();
            auto       t = magic_enum::enum_cast<WEB_HOST_INTERNAL_CMD_TYPE>( cmd.type );

            if( !t.has_value() )
            {
                wxLogDebug( "Invalid message received: %s", msg );
                return;
            }

            switch( *t )
            {
            case WEB_HOST_INTERNAL_CMD_TYPE::fetch_global_context_from_host:
            {
                if( _host_copilot_handles.global_context_handle.expired() )
                {
                    wxLogDebug( "Get design global context handle expired" );
                    break;
                }

                auto context_function = _host_copilot_handles.global_context_handle.lock();

                auto& global_ctx = ( *context_function )();

                if( _consumed_global_ctx_keys.contains( global_ctx.uuid ) )
                    break;

                auto global_ctx_value = global_ctx.dump();
                auto ctx_id = global_ctx.uuid;

                wxString out;
                _browser->RunScriptAsync(
                        fmt::format( "try {{ {}({}); }} catch (e) {{ console.error('JS "
                                     "exception:', e); }}",
                                     magic_enum::enum_name(
                                             WEBVIEW_FUNCTIONS::update_copilot_global_context ),
                                     global_ctx_value ),
                        &out );


                _consumed_global_ctx_keys.insert( ctx_id );

                break;
            }
            case WEB_HOST_INTERNAL_CMD_TYPE::show_image:
            {
                if( !_image_viewer )
                {
                    const auto geometry =
                            COPILOT_SETTINGS_MANAGER::get_instance().get_image_viewer_geometry();
                    _image_viewer = new DIALOG_IMAGE_VIEWER( m_parent, wxID_ANY, "Image Viewer",
                                                             { geometry.x, geometry.y },
                                                             { geometry.width, geometry.height } );
                }
                _image_viewer->reload();
                _image_viewer->Show();

                break;
            }
            }
            break;
        }
        case PA_AGENT:
        {
            if( !_host_copilot_handles.agent_action_handle.expired() )
            {
                auto context_function = _host_copilot_handles.agent_action_handle.lock();
                ( *context_function )( act_container.data );
            }
            break;
        }
        }
    }
    catch( const std::exception& e )
    {
        wxLogDebug( "Invalid message received: %s , %s", msg, e.what() );
    }
}

void WEBVIEW_CONTAINER::generic_process_web_msg_wrapper( std::string const& msg )
{
    CallAfter(
            [this, msg]()
            {
                generic_process_web_msg( msg );
            } );
}
