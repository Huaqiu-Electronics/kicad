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

MCP_CLIENT_WORKER::MCP_CLIENT_WORKER( wxEvtHandler& eventSink, MCP_CLIENT_REQUEST_QUEUE& requestQueue ) :
        m_eventSink( eventSink ), m_requestQueue( requestQueue )
{
}

MCP_CLIENT_WORKER::~MCP_CLIENT_WORKER()
{
}

void MCP_CLIENT_WORKER::Start()
{
    nng_socket sock;

    if( nng_pair0_open( &sock ) != 0 )
    {
        wxLogError( "nng_pair0_open" );
        return;
    }

    if( nng_dial( sock, "ipc:///tmp/kicad_copilot_pair.ipc", NULL, 0 ) != 0 )
    {
        wxLogError( "nng_dial" );
        return;
    }

    if( nng_setopt_ms( sock, NNG_OPT_RECVTIMEO, 100 ) != 0 )
    {
        wxLogError( "nng_setopt_ms" );
        return;
    }


    while( !m_shouldStop )
    {
        std::string request;

        if( m_requestQueue.ReceiveTimeout( 0, request ) == wxMSGQUEUE_NO_ERROR )
        {
            try
            {
                const auto cmd_t = magic_enum::enum_cast<MCP_CMD_TYPE>(
                                           nlohmann::json::parse( request ).get<MCP_CMD_BASE>().cmd_type )
                                           .value_or( MCP_CMD_TYPE::invalid );

                if( cmd_t == MCP_CMD_TYPE::invalid )
                {
                    wxLogError( "Invalid command type" );
                    continue;
                }

                nng_send( sock, request );

                for( ;; )
                {
                    if( auto res = recv_result( sock ) )
                    {
                        auto event = new MCP_CLIENT_EVENT( EVT_MCP_COMPLETE_MSG );
                        event->SetString( *res );
                        m_eventSink.QueueEvent( event );
                        break;
                    }
                }
            }
            catch( std::exception& e )
            {
                wxLogError( "Exception: %s", e.what() );
            }
        }
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
