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

#ifndef PCB_AGENT_PY_PLUGIN_ARG_H
#define PCB_AGENT_PY_PLUGIN_ARG_H

#include <optional>
#include <nlohmann/json.hpp>
#include <wx/string.h>

class PCB_AGENT_PY_PLUGIN_ARG
{
public:
    static constexpr auto kNullJson = "null";

    static PCB_AGENT_PY_PLUGIN_ARG& Get()
    {
        static PCB_AGENT_PY_PLUGIN_ARG instance;
        return instance;
    }

    void SetPluginArgs( const std::optional<nlohmann::json>& aPluginArgs ) { m_pluginArgs = aPluginArgs; }

    wxString GetPluginArgs() const
    {
        return m_pluginArgs.has_value() ? wxString::FromUTF8( m_pluginArgs->dump().c_str() ) : wxString( kNullJson );
    }

private:
    PCB_AGENT_PY_PLUGIN_ARG() = default;

    std::optional<nlohmann::json> m_pluginArgs;
};

#endif
