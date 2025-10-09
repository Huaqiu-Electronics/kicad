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

#include "mcp_client_worker.h"
#include "proto/mcp_cmd_type.h"
#include "proto/mcp_cmd_base.h"
#include "mcp_client_event.h"

#include <atomic>
#include <optional>
#include <string>
#include <thread>
#include <utils/generate_uuid.h>
#include <nng/nng.h>
#include <nng/protocol/pair0/pair.h>
#include <fmt/format.h>
#include <wx/log.h>
#include <magic_enum.hpp>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/process.h>


inline auto get_exe_dir()
{
    return wxFileName( wxStandardPaths::Get().GetExecutablePath() ).GetPath();
}


inline auto get_py_exe_path()
{
    static const auto py_exe_path = get_exe_dir() + wxFileName::GetPathSeparator() + "python";
    return py_exe_path;
}

long MCP_CLIENT_WORKER::launch_mcp_py_client( std::string const& url )
{
    // Get python executable path
    auto py_exe = get_py_exe_path();

    // Compose the command: python ./kicad-mcp-client <url>
    wxString script_path = get_exe_dir() + wxFileName::GetPathSeparator() + "kicad-mcp-client";
    wxString cmd = wxString::Format( "\"%s\" \"%s\" \"%s\"", py_exe, script_path, url );

    // Launch the process (non-blocking)
    long pid = wxExecute( cmd,

#ifndef NDEBUG

                          wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE
#else
                          wxEXEC_ASYNC

#endif // !NDEBUG


    );

    if( pid == 0 )
    {
        wxLogError( "Failed to launch MCP Python client: %s", cmd );
    }

    return pid;
}
// ...existing code...
inline auto recv_result( nng_socket& sock )
{
    char*  buf = nullptr;
    int    rv{};
    size_t sz{};
    rv = nng_recv( sock, &buf, &sz, NNG_FLAG_ALLOC );
    std::optional<std::string> result;

    if( rv == 0 && buf )
    {
        result = std::string( buf, sz );

        nng_free( buf, sz );
    }
    return result;
}


inline auto nng_send( nng_socket& sock, std::string const& msg )
{
    for( auto i = 0; i < 3; i++ )
    {
        if( nng_send( sock, const_cast<char*>( msg.c_str() ), msg.length(), 0 ) == 0 )
        {
            break;
        }

        std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
    }
}

MCP_CLIENT_WORKER::MCP_CLIENT_WORKER( wxEvtHandler& eventSink, MCP_CLIENT_REQUEST_QUEUE& requestQueue,
                                      std::string aUrl ) :
        m_eventSink( eventSink ), m_requestQueue( requestQueue ), m_url( std::move( aUrl ) )
{
}

MCP_CLIENT_WORKER::~MCP_CLIENT_WORKER()
{
}

void MCP_CLIENT_WORKER::Start()
{
    enum
    {
        REC_SEND_TIMEOUT_MS = 100
    };

    nng_socket sock;

    if( nng_pair0_open( &sock ) != 0 )
    {
        wxLogError( "nng_pair0_open" );
        return;
    }

    if( nng_dial( sock, m_url.c_str(), NULL, 0 ) != 0 )
    {
        wxLogError( "nng_dial" );
        return;
    }

    if( nng_socket_set_ms( sock, NNG_OPT_RECVTIMEO, REC_SEND_TIMEOUT_MS ) != 0 )
    {
        wxLogError( "nng_socket_set_ms NNG_OPT_RECVTIMEO" );
        return;
    }


    const auto pull_request = [&]
    {
        if( m_requestQueue.ReceiveTimeout( REC_SEND_TIMEOUT_MS, m_request ) != wxMSGQUEUE_NO_ERROR )
        {
            return;
        }

        try
        {
            const auto cmd_t = magic_enum::enum_cast<MCP_CMD_TYPE>(
                                       nlohmann::json::parse( m_request ).get<MCP_CMD_BASE>().cmd_type )
                                       .value_or( MCP_CMD_TYPE::invalid );

            if( cmd_t == MCP_CMD_TYPE::invalid )
            {
                wxLogError( "Invalid command type: %s", m_request );
                return;
            }

            nng_send( sock, m_request );
        }
        catch( std::exception& e )
        {
            wxLogError( "Exception: %s", e.what() );
        }
    };

    const auto fetch_response = [&]
    {
        auto res = recv_result( sock );

        if( !res )
        {
            return;
        }

        auto event = new MCP_CLIENT_EVENT( EVT_MCP_COMPLETE_MSG );
        event->SetString( *res );
        m_eventSink.QueueEvent( event );
    };


    while( !m_shouldStop )
    {
        pull_request();
        fetch_response();
    }

    static const auto kQuit =
            nlohmann::json( MCP_CMD_BASE{ magic_enum::enum_name( MCP_CMD_TYPE::quit ).data() } ).dump();
    nng_send( sock, kQuit );
    // Wait for the msg to be received by the other side
    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

    nng_close( sock );
}

void MCP_CLIENT_WORKER::Stop()
{
    m_shouldStop = true;
}

std::string MCP_CLIENT_WORKER::fmt_client_url()
{
    return fmt::format( "ipc:///tmp/kicad_copilot_pair-{}.ipc", generate_uuid() );
}
