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

#ifndef FMT_PCB_SELECTION_CMD_H
#define FMT_PCB_SELECTION_CMD_H

#include <chat/general_chat_cmd.h>
#include <context/pcb/details/board_json_schema.h>
#include <chat/user_ask_selection_cmd.h>
#include <fmt/format.h>

inline auto fmt_pcb_selection_cmd( nlohmann::json const& aContext, std::string&& pcb )
{
    return nlohmann::json( GENERAL_CHAT_CMD{ { { aContext.get<std::string>() } },
                                             { { fmt::format( "The user is working on a pcb board design, and has "
                                                              "just selected some items on the board. All the "
                                                              "selected items are serialized into the "
                                                              "the following json :\n{}. It's "
                                                              "defined "
                                                              "following the json schema:\n {}\n"
                                                              "NOTE: You always provide clear answers "
                                                              "rather than asking for more information.\n "
                                                              "Please note that our users are hardware engineers who "
                                                              "are neither aware nor interested in the existence of "
                                                              "this JSON data. Never expose the existence of a JSON "
                                                              "file.",
                                                              pcb, copilot::BOARD_JSON_SCHEMA ) } },
                                             kUserAskSelectionCmdType } )
            .dump();
}

#endif
