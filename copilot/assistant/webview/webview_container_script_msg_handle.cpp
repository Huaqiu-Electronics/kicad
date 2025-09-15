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
#include "internal/modal_dialog/end_modal.h"
#include "internal/modal_dialog/show_modal.h"

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
#include <active_action/common/selection_change_cmd.h>
#include <webview/webview_script_msg_handler.h>
#include <webview/webview_functions.h>
#include <webview/webview_controller.h>
#include <auth/dialog_login.h>
#include <context/copilot_global_context_handle.h>
#include <context/copilot_global_context.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <wx/timer.h>
#include <utils/httplib_wrapper.h>
#include <utils/extract_host_from_url.h>
#include <wx/dialog.h>
#include <common/dialog_image_viewer.h>
#include <webview_library/dialog_add_component.h>
#include <webview/launch_webview_dialog_param.h>


void WEBVIEW_CONTAINER::HandleScriptMessage( std::string const& msg )
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


                if( auto dialog = dynamic_cast<wxDialog*>( GetParent() ) )
                {
                    dialog->EndModal( wxID_OK );
                }


                WEBVIEW_SETTINGS_MANAGER::get_instance().set_access_token( *cmd.data );

                break;
            }

            case WEB_HOST_INTERNAL_CMD_TYPE::launch_login_dialog:
            {
                DIALOG_LOGIN dialog( GetParent() );
                dialog.ShowModal();

                if( !dialog.Success() )
                {
                    WEBVIEW_CONTROLLER::Get().OnLoginFailed( this );
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
            case WEB_HOST_INTERNAL_CMD_TYPE::add_component:
            case WEB_HOST_INTERNAL_CMD_TYPE::add_component_type:
            case WEB_HOST_INTERNAL_CMD_TYPE::add_design_block_type:
            {
                static const std::map<WEB_HOST_INTERNAL_CMD_TYPE, std::string> type_to_url = {
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_component,
                      WEBVIEW_SETTINGS_MANAGER::get_instance().get_add_component_url() },
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_component_type,
                      WEBVIEW_SETTINGS_MANAGER::get_instance().get_add_component_type_url() },
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_design_block_type,
                      WEBVIEW_SETTINGS_MANAGER::get_instance().get_add_design_block_type_url() }

                };

                static const std::map<WEB_HOST_INTERNAL_CMD_TYPE, wxString> type_to_name = {
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_component, _( "Add Component" ) },
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_component_type, _( "Add Component Type" ) },
                    { WEB_HOST_INTERNAL_CMD_TYPE::add_design_block_type, _( "Add Design Block Type" ) }

                };

                PREFERRED_SIZE size;

                try
                {
                    if( cmd.data.has_value() )
                        size = cmd.data->get<LAUNCH_WEBVIEW_DIALOG_PARAM>().size;
                }
                catch( ... )
                {
                }


                DIALOG_ADD_COMPONENT dialog( GetParent(), type_to_url.at( *t ), type_to_name.at( *t ),
                                             magic_enum::enum_name( *t ).data(), { size.width, size.height } );
                dialog.ShowModal();
                break;
            }

            case WEB_HOST_INTERNAL_CMD_TYPE::show_modal:
            {
                const auto     show_model = cmd.data->get<SHOW_MODAL>();
                DIALOG_WEBVIEW modal_dialog( GetParent(), show_model.title, show_model.key,
                                             { show_model.size.width, show_model.size.height }, wxID_ANY,
                                             _impl->_host_copilot_handles, this );
                modal_dialog.LoadUrl( show_model.url );
                modal_dialog.ShowModal();
                break;
            }
            case WEB_HOST_INTERNAL_CMD_TYPE::end_modal:
            {
                if( auto parent = dynamic_cast<DIALOG_WEBVIEW*>( GetParent() ) )
                {
                    const auto end_modal = cmd.data->get<END_MODAL>();

                    if( end_modal.callback )
                    {
                        if( auto parent_web = parent->GetParentWebView() )
                        {
                            if( !end_modal.data )
                            {
                                parent_web->RunScriptAsync( fmt::format( "{}()", *end_modal.callback ) );
                            }
                            else
                            {
                                parent_web->RunScriptAsync(
                                        fmt::format( "{}({})", *end_modal.callback, end_modal.data->dump() ) );
                            }
                        }
                    }

                    parent->EndModal( wxID_OK );
                }
                else
                {
                    wxLogDebug( "Invalid message received: %s", msg );
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
