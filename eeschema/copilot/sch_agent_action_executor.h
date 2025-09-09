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

#ifndef SCH_AGENT_ACTION_EXECUTOR_H
#define SCH_AGENT_ACTION_EXECUTOR_H

#include <api/api_handler_sch.h>
#include <api/api_design_resource.h>
#include <api/api_design_content.h>
#include <api/api_server.h>
#include <pgm_base.h>
#include <context/symbol_context.h>
#include <exception>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>
#include <passive_action/agent/context/component_agent_action_context.h>
#include <passive_action/agent/context/highlight_symbol_context.h>
#include <sch_edit_frame.h>
#include <magic_enum.hpp>
#include <string>
#include <wx/log.h>
#include <active_action/sch/sch_copilot_cmd_type.h>
#include <webview/webview_container.h>
#include <chat/general_chat_cmd.h>
#include <context/sch/details/sch_netlist.h>
#include <fmt/format.h>
#include <api/api_sch_placement_command.h>
#include <utils/wstring_to_string.h>
#include <webview_library/dialog_add_design_block.h>
#include <webview/launch_webview_dialog_param.h>

#include <api/decode_base64_encoded_component.h>
#include <api/api_place_component_cmd.h>

#include "sch_editor_control.h"
#include "fmt_sch_selection_cmd.h"

template <>
void SCH_COPILOT_PANEL_CONTAINER::ExecuteAgentAction( AGENT_ACTION const& aAction )
{
    try
    {
        auto t = magic_enum::enum_cast<AGENT_ACTION_TYPE>( aAction.action );

        if( !t.has_value() )
        {
            wxLogWarning( "Unknown action received: %s", aAction.action.c_str() );
            return;
        }

        switch( *t )
        {
        case AGENT_ACTION_TYPE::highlight_symbol:
        {
            self()->HighlightSymbol( aAction.context.get<HIGHLIGHT_SYMBOL_CONTEXT>().designator );
            break;
        }

        case AGENT_ACTION_TYPE::ask_selection:
        {
            m_copilotPanel->ExecuteCommand(
                    fmt_sch_selection_cmd( aAction.context, nlohmann::json( self()->GetSelection() ).dump() ).c_str() );
            break;
        }
        case AGENT_ACTION_TYPE::place_symbol_resource:
        {
            self()->m_apiHandler->placeSymbol( aAction.context.get<PLACE_SYMBOL_RESOURCE_COMMAND>() );
            break;
        }
        case AGENT_ACTION_TYPE::place_symbol_content:
        {
            const auto content = aAction.context.get<PLACE_SYMBOL_CONTENT_COMMAND>();
            self()->m_apiHandler->placeSymbol( content.component, content.option );
            break;
        }

        case AGENT_ACTION_TYPE::place_base64_encoded_symbol:
        {
            auto content = aAction.context.get<PLACE_SYMBOL_CONTENT_COMMAND>();
            decode_base64_encoded_component( content.component );
            self()->m_apiHandler->placeSymbol( content.component, content.option );
            break;
        }

        case AGENT_ACTION_TYPE::place_design_block_resource:
        {
            self()->m_apiHandler->placeDesignBlock( aAction.context.get<PLACE_DESIGN_BLOCK_RESOURCE_COMMAND>() );
            break;
        }
        case AGENT_ACTION_TYPE::place_design_block_content:
        {
            const auto content = aAction.context.get<PLACE_DESIGN_BLOCK_CONTENT_COMMAND>();
            self()->m_apiHandler->placeDesignBlock( content.design_block, content.option );
            break;
        }

        case AGENT_ACTION_TYPE::place_component_resource:
        {
            self()->m_apiHandler->placeSymbol( PLACE_SYMBOL_RESOURCE_COMMAND{
                    aAction.context.get<PLACE_COMPONENT_RESOURCE_COMMAND>().component } );
            break;
        }
        case AGENT_ACTION_TYPE::place_base64_encoded_component:
        {
            auto cmd = aAction.context.get<PLACE_COMPONENT_CONTENT_COMMAND>();

            if( cmd.base64_encoded_content )
            {
                decode_base64_encoded_component( cmd.component );
            }

            self()->m_apiHandler->placeSymbol( cmd.component, {} );
            break;
        }

        case AGENT_ACTION_TYPE::add_design_block:
        {
            const auto content = self()->DumpSelectionToDesignBlock();

            if( content.IsEmpty() )
            {
                return;
            }

            PREFERRED_SIZE size;

            try
            {
                size = aAction.context.get<LAUNCH_WEBVIEW_DIALOG_PARAM>().size;
            }
            catch( ... )
            {
            }

            DIALOG_ADD_DESIGN_BLOCK dialog( self(), wxString2String( std::move( content ) ),
                                            { size.width, size.height } );
            dialog.ShowModal();
            break;
        }

        default: return BaseExecuteAgentAction( aAction );
        }
    }
    catch( std::exception const& e )
    {
        wxLogError( "Exception caught: %s while processing action %s:", e.what(),
                    nlohmann::json( aAction ).dump().c_str() );
    }
}


#endif
