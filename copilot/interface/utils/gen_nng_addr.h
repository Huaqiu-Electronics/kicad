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

#ifndef GEN_NNG_ADDR_H
#define GEN_NNG_ADDR_H

#include <optional>
#include <string>
#include <fmt/format.h>
#include <utils/generate_uuid.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

// We expect to expose a deterministic address to the MCP client so that it can connect to our SDK server, and we want to avoid potential conflicts with other instances of the client or server that may be running. On Windows, we can use the process ID and an optional editor name to create a unique address. On Unix-like systems, we can use a similar approach but with a file in the /tmp directory. The editor name is optional but can help further distinguish between different instances, especially if multiple editors are running on the same machine.

inline auto gen_nng_addr( std::optional<std::string> const& editor_name = {} )
{
#ifdef _WIN32
    return fmt::format( "ipc://kicad_copilot_pair-{}-{}", _getpid(), editor_name.value_or( generate_uuid() ) );
#else
    return fmt::format( "ipc:///tmp/kicad_copilot_pair-{}-{}.ipc", getpid(), editor_name.value_or( generate_uuid() ) );
#endif
}
#endif
