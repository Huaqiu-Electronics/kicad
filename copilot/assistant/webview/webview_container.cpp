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
#include <nlohmann/json.hpp>
#include <settings/webview_settings_manager.h>
#include <settings/dialog_geometry.h>
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
#include <utils/httplib_wrapper.h>
#include <utils/wstring_to_string.h>
#include <utils/extract_host_from_url.h>
#include <wx/dialog.h>

#include "http/http_client.h"
#include <common/dialog_image_viewer.h>
#include "webview_constant.h"

#ifndef _WIN32
#include "webview_evt_filter.h"
#endif


using json = nlohmann::json;
using namespace httplib;

inline auto send_data_buried_point( std::string const& name )
{
    std::thread t(
            [=]()
            {
                try
                {
                    HTTP_CLIENT http_client;
                    http_client.send_data_buried_point( DATA_BURIED_POINT{ name, fmt::format( "{}_in_kicad", name )

                    } );
                }
                catch( std::exception const& e )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: %s", e.what() ),
                                "send_data_buried_point" );
                }
                catch( ... )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: unknown" ), "send_data_buried_point" );
                }
            } );
    t.detach();
}


enum START_UP_SIZE
{
    WIDTH = 400,
    HEIGHT = 1040
};


struct WEBVIEW_CONTAINER_IMPL
{
    std::set<std::string> _consumed_global_ctx_keys{};
    DIALOG_IMAGE_VIEWER*  _image_viewer{};
    DIALOG_WEBVIEW*       _ecad_viewer{};


    wxWebView*                       _browser;
    HOST_COPILOT_HANDLES             _host_copilot_handles;
    std::string                      _primary_url;
    std::shared_ptr<httplib::Server> _web_msg_server;
    std::unique_ptr<std::thread>     _web_msg_server_thread;
};


WEBVIEW_CONTAINER::WEBVIEW_CONTAINER( wxWindow* parent, std::string url, HOST_COPILOT_HANDLES host_copilot_handles,
                                      std::optional<std::string>       data_buried_point,
                                      std::shared_ptr<httplib::Server> msgServer ) :
        wxPanel( parent ), _impl( std::make_unique<WEBVIEW_CONTAINER_IMPL>() )

{
    _impl->_host_copilot_handles = std::move( host_copilot_handles );
    _impl->_web_msg_server = std::move( msgServer );
    _impl->_primary_url = std::move( url );
    _impl->_browser = wxWebView::New();


    auto top_sizer = new wxBoxSizer( wxVERTICAL );
    _impl->_browser->Create( this, wxID_ANY, _impl->_primary_url, wxDefaultPosition, wxDefaultSize );
    top_sizer->Add( _impl->_browser, wxSizerFlags().Expand().Proportion( 1 ) );


    if( _impl->_web_msg_server )
    {
        _impl->_web_msg_server_thread = std::make_unique<std::thread>(
                [&]()
                {
                    // Handle POST requests
                    _impl->_web_msg_server->Post( "/passive_action",
                                                  [this]( const Request& req, Response& res )
                                                  {
                                                      // Add CORS headers
                                                      res.set_header( "Access-Control-Allow-Origin", "*" );
                                                      res.set_header( "Access-Control-Allow-Methods", "POST, OPTIONS" );
                                                      res.set_header( "Access-Control-Allow-Headers", "*" );

                                                      const auto msg = req.body;
                                                      if( msg.empty() )
                                                      {
                                                          res.set_content( "Invalid message", "text/plain" );
                                                          return;
                                                      }

                                                      // Process the message
                                                      OnWebChannelMsgWrapper( msg );
                                                      res.set_content( "OK", "text/plain" );
                                                  } );

                    // Handle OPTIONS preflight requests
                    _impl->_web_msg_server->Options( "/passive_action",
                                                     []( const Request& req, Response& res )
                                                     {
                                                         res.set_header( "Access-Control-Allow-Origin", "*" );
                                                         res.set_header( "Access-Control-Allow-Methods",
                                                                         "POST, OPTIONS" );
                                                         res.set_header( "Access-Control-Allow-Headers", "*" );
                                                         res.set_content( "", "text/plain" );
                                                     } );


                    _impl->_web_msg_server->listen_after_bind();
                } );
    }

    if( !_impl->_browser->AddScriptMessageHandler( magic_enum::enum_name( WEBVIEW_MSG_HANDLES::eda_host ).data() ) )
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

    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WEBVIEW_CONTAINER::OnScriptMessage, this, _impl->_browser->GetId() );
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
}

WEBVIEW_CONTAINER::~WEBVIEW_CONTAINER()
{
    if( _impl->_web_msg_server )
        _impl->_web_msg_server->stop();

    if( _impl->_web_msg_server_thread )
        _impl->_web_msg_server_thread->join();

#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().RemovePanel( this );
#endif
}

void WEBVIEW_CONTAINER::ExecuteCommand( const char* cmd )
{
    const auto js_call = fmt::format( "try {{ {}({}); }} catch (e) {{ console.error('JS exception:', e); }}",
                                      magic_enum::enum_name( WEBVIEW_FUNCTIONS::fire_host_active_cmd ), cmd );

    CallAfter(
            [this, js_call]()
            {
                wxString out;
                _impl->_browser->RunScriptAsync( js_call, &out );
                wxLogDebug( "ExecuteCommand (wrapped): %s", out );
            } );
}


void WEBVIEW_CONTAINER::OnHostSelectionChanged( bool has_selection )
{
    ExecuteCommand( json( SELECTION_CHANGE_CMD{ has_selection } ).dump().c_str() );
}


WEBVIEW_MSG_CHANNELS WEBVIEW_CONTAINER::MsgChannelType() const
{
    return _impl->_web_msg_server != nullptr ? WEBVIEW_MSG_CHANNELS::HTTP : WEBVIEW_MSG_CHANNELS::POST_MSG;
}


void WEBVIEW_CONTAINER::OnNavigationRequest( wxWebViewEvent& evt )
{
    const auto url = evt.GetURL();

    if( !url.Contains( extract_host_from_url( _impl->_primary_url ) ) )
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


// NOTE : This is the entrypoint for HQ distribution v9.0.2 , which has been released on 2025-05
// The msg is posted through _browser->AddScriptMessageHandler( "eda_host" )
// Hover, it will lead to crash on MacOS , so the http way is preferred
void WEBVIEW_CONTAINER::OnScriptMessage( wxWebViewEvent& evt )
{
    OnWebChannelMsg( wxString2String( evt.GetString() ) );
}


void WEBVIEW_CONTAINER::OnScriptResult( wxWebViewEvent& evt )
{
    if( evt.IsError() )
        wxLogDebug( "Async JS failed: %s", evt.GetString() );
    else
        wxLogDebug( "Async JS succeeded: %s", evt.GetString() );
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

void WEBVIEW_CONTAINER::OnWebChannelMsg( std::string const& msg )
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
                if( _impl->_host_copilot_handles.global_context_handle.expired() )
                {
                    wxLogDebug( "Get design global context handle expired" );
                    break;
                }

                auto context_function = _impl->_host_copilot_handles.global_context_handle.lock();

                auto& global_ctx = ( *context_function )();

                if( _impl->_consumed_global_ctx_keys.contains( global_ctx.uuid ) )
                    break;

                auto global_ctx_value = global_ctx.dump();
                auto ctx_id = global_ctx.uuid;

                wxString out;
                _impl->_browser->RunScriptAsync(
                        fmt::format( "try {{ {}({}); }} catch (e) {{ console.error('JS "
                                     "exception:', e); }}",
                                     magic_enum::enum_name( WEBVIEW_FUNCTIONS::update_copilot_global_context ),
                                     global_ctx_value ),
                        &out );


                _impl->_consumed_global_ctx_keys.insert( ctx_id );

                break;
            }
            case WEB_HOST_INTERNAL_CMD_TYPE::show_image:
            {
                if( !_impl->_image_viewer )
                {
                    _impl->_image_viewer =
                            new DIALOG_IMAGE_VIEWER( m_parent, wxID_ANY, _( "Image Viewer" ), "image_viewer" );
                    _impl->_image_viewer->LoadUrl(
                            WEBVIEW_SETTINGS_MANAGER::get_instance().get_webview_image_viewer_path() );
                }

                _impl->_image_viewer->reload();
                _impl->_image_viewer->Show();

                break;
            }
            case WEB_HOST_INTERNAL_CMD_TYPE::show_ecad_viewer:
            {
                if( cmd.data )
                {
                    try
                    {
                        const auto url = cmd.data->get<std::string>();

                        if( !_impl->_ecad_viewer )
                        {
                            _impl->_ecad_viewer =
                                    new DIALOG_WEBVIEW( m_parent, wxID_ANY, _( "ECAD Viewer" ), "ecad_viewer" );
                        }

                        _impl->_ecad_viewer->LoadUrl( url );
                        _impl->_ecad_viewer->Show();
                    }
                    catch( ... )
                    {
                    }
                }
                break;
            }
            }
            break;
        }
        case PA_AGENT:
        {
            if( !_impl->_host_copilot_handles.agent_action_handle.expired() )
            {
                auto context_function = _impl->_host_copilot_handles.agent_action_handle.lock();
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

void WEBVIEW_CONTAINER::OnWebChannelMsgWrapper( std::string const& msg )
{
    CallAfter(
            [this, msg]()
            {
                OnWebChannelMsg( msg );
            } );
}


std::string WEBVIEW_CONTAINER::GetUrl() const
{
    return _impl->_primary_url;
}

wxWebView* WEBVIEW_CONTAINER::GetWebView() const
{
    return _impl->_browser;
}
