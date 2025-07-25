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

#include <auth/access_token.h>
#include <auth/dialog_login.h>
#include <wx/event.h>
#include <settings/webview_settings_manager.h>
#include <webview/webview_constant.h>
#include <wx/log.h>
#include <passive_action/web/web_host.h>
#include <passive_action/passive_action_container.h>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>
#include <magic_enum.hpp>
#include <utils/extract_host_from_url.h>
#include <utils/web_utils.h>
#include <utils/wstring_to_string.h>

#ifndef _WIN32
#include <webview/webview_evt_filter.h>
#endif

enum FIXED_SIZE
{
    WIDTH = 200,
    HEIGHT = 600
};

DIALOG_LOGIN::DIALOG_LOGIN( wxWindow* aParent ) : DIALOG_WEBVIEW( aParent, wxID_ANY, _( "Login" ), "dialog_login" )
{
    LoadUrl( WEBVIEW_SETTINGS_MANAGER::get_instance().get_login_path() );

    auto _browser = &GetBrowser();

    if( !!_browser->AddScriptMessageHandler( magic_enum::enum_name( WEBVIEW_MSG_HANDLES::eda_host ).data() ) )
    {
        wxLogDebug( "Could not add script message handler " );
    }

    SetMinSize( FromDIP( wxSize( FIXED_SIZE::WIDTH, FIXED_SIZE::HEIGHT ) ) );
    SetMaxSize( FromDIP( wxSize( FIXED_SIZE::WIDTH, FIXED_SIZE::HEIGHT ) ) );

    Bind( wxEVT_WEBVIEW_NAVIGATING, &DIALOG_LOGIN::OnNavigationRequest, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_ERROR, &DIALOG_LOGIN::OnError, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_NEWWINDOW, &DIALOG_LOGIN::OnNewWindow, this, _browser->GetId() );

    Bind( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &DIALOG_LOGIN::OnScriptMessage, this, _browser->GetId() );
    Bind( wxEVT_WEBVIEW_SCRIPT_RESULT, &DIALOG_LOGIN::OnScriptResult, this, _browser->GetId() );

#ifdef DEBUG

    _browser->EnableAccessToDevTools( true );

#endif // DEBUG

#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().AddPanel( this );
#endif
}

DIALOG_LOGIN::~DIALOG_LOGIN()
{
#ifndef _WIN32
    WEBVIEW_EVT_FILTER::Get().RemovePanel( this );
#endif
}


void DIALOG_LOGIN::OnNavigationRequest( wxWebViewEvent& evt )
{
    const auto url = evt.GetURL();

    if( !url.Contains( extract_host_from_url( WEBVIEW_SETTINGS_MANAGER::get_instance().get_login_path() ) ) )
    {
        // Launch the url in the system default browser
        wxLaunchDefaultBrowser( url );
        evt.Veto();
        return;
    }
}


void DIALOG_LOGIN::OnNewWindow( wxWebViewEvent& evt )
{
    GetBrowser().LoadURL( evt.GetURL() );
    evt.Veto(); // Prevent WebKit from creating a new page
}


void DIALOG_LOGIN::OnScriptMessage( wxWebViewEvent& evt )
{
    OnWebChannelMsg( wxString2String( evt.GetString() ) );
}


void DIALOG_LOGIN::OnScriptResult( wxWebViewEvent& evt )
{
    if( evt.IsError() )
        wxLogDebug( "Async JS failed: %s", evt.GetString() );
    else
        wxLogDebug( "Async JS succeeded: %s", evt.GetString() );
}

void DIALOG_LOGIN::OnError( wxWebViewEvent& evt )
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

void DIALOG_LOGIN::OnWebChannelMsg( std::string const& msg )
{
    try
    {
        const auto act_container = nlohmann::json::parse( msg ).get<PASSIVE_ACTION_CONTAINER>();
        switch( act_container.category )
        {
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
            case WEB_HOST_INTERNAL_CMD_TYPE::update_access_token:
            {
                if( !cmd.data.has_value() )
                {
                    wxLogDebug( "Invalid message received: %s", msg );
                    return;
                }
                auto token = cmd.data->get<ACCESS_TOKEN>();
                WEBVIEW_SETTINGS_MANAGER::get_instance().set_access_token( token );
                break;
            }
            default:
            {
                wxLogDebug( "Invalid message received: %s", msg );
                break;
            }
            }
            break;
        }
        default:
        {
            wxLogDebug( "Invalid message received: %s", msg );
            break;
        }
        }
    }
    catch( const std::exception& e )
    {
        wxLogDebug( "Invalid message received: %s , %s", msg, e.what() );
    }
}

void DIALOG_LOGIN::OnWebChannelMsgWrapper( std::string const& msg )
{
    CallAfter(
            [this, msg]()
            {
                OnWebChannelMsg( msg );
            } );
}
