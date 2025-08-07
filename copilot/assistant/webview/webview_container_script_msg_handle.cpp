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
#include <settings/webview_settings_manager.h>
#include "webview/internal/open_url_in_dialog.h"
#include "webview/internal/webview_internal_cmd_type.h"
#include "webview/internal/webview_internal_cmd.h"
#include <passive_action/passive_action_container.h>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>
#include <active_action/cmd/common/selection_change_cmd.h>
#include <webview/webview_token_controller.h>
#include <webview/webview_script_msg_handler.h>
#include <webview/webview_functions.h>

#include <auth/dialog_login.h>


#include <context/copilot_global_context_handle.h>
#include <context/copilot_global_context.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <wx/timer.h>
#include <utils/httplib_wrapper.h>
#include <utils/wstring_to_string.h>
#include <utils/extract_host_from_url.h>
#include <wx/dialog.h>

#include "webview_container_impl.h"


#include <common/dialog_image_viewer.h>

void WEBVIEW_CONTAINER::OnScriptMessage( wxWebViewEvent& evt )
{
    const auto msg = wxString2String( evt.GetString() );
    if( !_impl->_host_copilot_handles.webview_msg_handle.expired() )
    {
        if( ( *_impl->_host_copilot_handles.webview_msg_handle.lock() )( evt ) )
        {
            return;
        }
    }

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
                    _impl->_image_viewer = new DIALOG_IMAGE_VIEWER( m_parent, _( "Image Viewer" ), "image_viewer" );
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
                            _impl->_ecad_viewer = new DIALOG_WEBVIEW( m_parent, _( "ECAD Viewer" ), "ecad_viewer" );
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
            case WEB_HOST_INTERNAL_CMD_TYPE::update_access_token:
            {
                if( !cmd.data.has_value() )
                {
                    wxLogDebug( "Invalid message received: %s", msg );
                    return;
                }

                WEBVIEW_SETTINGS_MANAGER::get_instance().set_access_token( *cmd.data );

                if( auto dialog = dynamic_cast<wxDialog*>( GetParent() ) )
                {
                    dialog->EndModal( wxID_OK );
                }


                break;
            }

            case WEB_HOST_INTERNAL_CMD_TYPE::launch_login_dialog:
            {
                DIALOG_LOGIN dialog( GetParent() );
                dialog.ShowModal();

                if( !dialog.Success() )
                {
                    WEBVIEW_TOKEN_CONTROLLER::Get().OnLoginFailed( this );
                    return;
                }

                break;
            }

            case WEB_HOST_INTERNAL_CMD_TYPE::logout:
            {
                WEBVIEW_SETTINGS_MANAGER::get_instance().clear_token();
                break;
            }

            case WEB_HOST_INTERNAL_CMD_TYPE::open_url_in_webview_dialog:
            {
                if( !cmd.data.has_value() )
                {
                    wxLogDebug( "Invalid message received: %s", msg );
                    return;
                }
                auto url = cmd.data->get<OPEN_URL_IN_WEBVIEW_DIALOG>().url;
                wxLaunchDefaultBrowser( url );
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
