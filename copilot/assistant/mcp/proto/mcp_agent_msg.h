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

#ifndef MCP_AGENT_MSG_H
#define MCP_AGENT_MSG_H

#include <nlohmann/json.hpp>


enum MCP_AGENT_MSG_TYPE
{
    INVALID,

    exception,
    complete,
    cnf_changed,

};

struct MCP_AGENT_MSG_BASE
{
    MCP_AGENT_MSG_TYPE type;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MCP_AGENT_MSG_BASE, type )
};


struct MCP_AGENT_EXCEPTION : MCP_AGENT_MSG_BASE
{
    std::string msg;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MCP_AGENT_EXCEPTION, type, msg )
};

struct MCP_AGENT_COMPLETE : MCP_AGENT_MSG_BASE
{
    std::string msg;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MCP_AGENT_COMPLETE, type, msg )
};

struct MCP_AGENT_CNF_CHANGED : MCP_AGENT_MSG_BASE
{
    std::map<std::string, nlohmann::json> servers_assets;
    nlohmann::json                        mcp_settings;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MCP_AGENT_CNF_CHANGED, type, servers_assets, mcp_settings )
};

#endif
