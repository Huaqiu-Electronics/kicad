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
#include "sch_editor_control.h"
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
#include <active_action/cmd/sch/sch_copilot_cmd_type.h>
#include <webview/webview_container.h>
#include <chat/general_chat_cmd.h>
#include <context/sch/details/sch_netlist.h>
#include <context/sch/details/netlist_json_schema.h>
#include <fmt/format.h>
#include <api/api_sch_placement_command.h>
#include <utils/wstring_to_string.h>
#include <chat/user_ask_selection_cmd.h>

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
            const auto netlist = nlohmann::json( self()->GetSelection() ).dump();
            m_copilotPanel->ExecuteCommand(
                    nlohmann::json(
                            GENERAL_CHAT_CMD{ { { aAction.context.get<std::string>() } },
                                              { { fmt::format( "The user is working on a schematic the "
                                                               "current selection of which is represented by "
                                                               "the following netlist:\n{}. The "
                                                               "netlist is defined "
                                                               "following the json schema:\n {}\n"
                                                               "NOTE: All the connections for any component "
                                                               "presented inside the components have been "
                                                               "included in the nets.\n"
                                                               "NOTE: You always provide clear answers "
                                                               "rather than asking for more information.\n "
                                                               "Please note that our users are hardware engineers who "
                                                               "are neither aware nor interested in the existence of "
                                                               "this JSON data. Never expose the existence of a JSON "
                                                               "file",
                                                               netlist, copilot::NETLIST_JSON_SCHEMA ) } },
                                              kUserAskSelectionCmdType } )
                            .dump()
                            .c_str() );
            break;
        }
        case AGENT_ACTION_TYPE::dump_selection_to_design_block:
        {
            const auto sch = wxString2String( self()->DumpSelectionToDesignBlock() );


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
