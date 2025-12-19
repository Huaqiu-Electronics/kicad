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

#include <mcp/fmt_mcp_msg_script.h>
#include <webview/webview_controller.h>
#include <webview/webview_container.h>
#include <webview/copilot_webview_container.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <auth/access_token.h>
#include <auth/auth_state.h>
#include <auth/auth_state_change.h>
#include <settings/webview_settings_manager.h>
#include <webview/webview_functions.h>
#include <wx/debug.h>
#include <utils/urls_equal.h>
#include <utils/wstring_to_string.h>
inline auto fmt_auth_state_change_script( AUTH_STATE state )
{
    const auto content =
            nlohmann::json(
                    AUTH_STATE_CHANGE{ state, WEBVIEW_SETTINGS_MANAGER::get_instance().get_raw_access_token() } )
                    .dump();


    const auto fn = magic_enum::enum_name( WEBVIEW_FUNCTIONS::handle_auth );
    return fmt::format( "typeof {} === 'function' && {}({});", fn, fn, content );
}


WEBVIEW_CONTROLLER::WEBVIEW_CONTROLLER() = default;

WEBVIEW_CONTROLLER::~WEBVIEW_CONTROLLER()
{
}

WEBVIEW_CONTROLLER& WEBVIEW_CONTROLLER::Get()
{
    static WEBVIEW_CONTROLLER instance;
    return instance;
};

void WEBVIEW_CONTROLLER::AddWebviewContainer( WEBVIEW_CONTAINER* aWebviewContainer )
{
    _webview_container_set.insert( aWebviewContainer );
}

void WEBVIEW_CONTROLLER::RemoveWebviewContainer( WEBVIEW_CONTAINER* aWebviewContainer )
{
    _webview_container_set.erase( aWebviewContainer );
}


void WEBVIEW_CONTROLLER::OnWebViewNavigationComplete( WEBVIEW_CONTAINER* aWebviewContainer )
{
    if( WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid() )
    {
        UpdateToken( aWebviewContainer );
    }
    else
    {
        ExpireToken( aWebviewContainer );
    }
}

void WEBVIEW_CONTROLLER::OnTokenUpdated()
{
    for( auto webview_container : _webview_container_set )
    {
        UpdateToken( webview_container );
    }
}


void WEBVIEW_CONTROLLER::OnLoginFailed( WEBVIEW_CONTAINER* aWebviewContainer )
{
    aWebviewContainer->RunScriptAsync( fmt_auth_state_change_script( AUTH_STATE::LOGIN_FAILED ) );
}

void WEBVIEW_CONTROLLER::ExpireToken( WEBVIEW_CONTAINER* aWebviewContainer )
{
    aWebviewContainer->RunScriptAsync( fmt_auth_state_change_script( AUTH_STATE::ACCESS_TOKEN_EXPIRED ) );
}

void WEBVIEW_CONTROLLER::UpdateToken( WEBVIEW_CONTAINER* aWebviewContainer )
{
    aWebviewContainer->RunScriptAsync( fmt_auth_state_change_script( AUTH_STATE::ACCESS_TOKEN_UPDATED ) );
}

void WEBVIEW_CONTROLLER::OnLogout()
{
    for( auto webview_container : _webview_container_set )
    {
        Logout( webview_container );
    }
}

void WEBVIEW_CONTROLLER::Logout( WEBVIEW_CONTAINER* aWebviewContainer )
{
    aWebviewContainer->RunScriptAsync( fmt_auth_state_change_script( AUTH_STATE::LOGOUT ) );
}

void WEBVIEW_CONTROLLER::RunScript( wxString const& aScript, std::string const& aUrlLoadedByWebview )
{
    for( const auto& webview_container : _webview_container_set )
    {
        if( webview_container )
        {
            if( const auto url = webview_container->GetUrl(); url )
            {
                if( urls_equal( aUrlLoadedByWebview, *url ) )
                {
                    webview_container->RunScriptAsync( aScript );
                }
            }
        }
    }
}

void WEBVIEW_CONTROLLER::OnMcpCnfChanged( wxString const& aCnf )
{
    for( auto& webview_container : _webview_container_set )
    {
        webview_container->RunScriptAsync( fmt_mcp_msg_script( wxString2String( aCnf.ToStdString() ) ) );
    }
}
