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

#include "kicad_sdk_server.h"
#include <nlohmann/json.hpp>
#include <context/copilot_global_context.h>
#include <passive_action/agent/agent_action.h>
#include <wx/log.h>
#include <magic_enum.hpp>
#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <wx/panel.h>
#include <utils/nng_request_server.h>
#include "proto/sdk_cmd_base.h"
#include "proto/sdk_agent_action.h"

KICAD_SDK_SERVER::KICAD_SDK_SERVER( HOST_COPILOT_HANDLES aHandles, wxPanel* aEvtSink, std::string aAddress ) :
        _handles( std::move( aHandles ) ), _evtSink( aEvtSink ), _address( std::move( aAddress ) )
{
    _server_thread = std::thread( &KICAD_SDK_SERVER::run_server, this );
    _server_thread.detach();
}

KICAD_SDK_SERVER::~KICAD_SDK_SERVER()
{
    m_shouldStop = true;
}


void KICAD_SDK_SERVER::run_server()
{
    using json = nlohmann::json;
    auto request_server = std::make_unique<NNG_REQUEST_SERVER>( _address );

    request_server->SetCallback(
            [this, &request_server]( std::string* aMessage )
            {
                try
                {
                    auto        request = json::parse( *aMessage );
                    std::string response;

                    // Parse base command
                    auto base_cmd = request.get<SDK_CMD_BASE>();
                    const std::string& cmd_str = base_cmd.cmd;

                    enum class SDK_CMD_TYPE
                    {
                        invalid,
                        netlist,
                        placeNetLabels
                    };

                    const auto cmd =
                            magic_enum::enum_cast<SDK_CMD_TYPE>( cmd_str ).value_or( SDK_CMD_TYPE::invalid );

                    if( cmd == SDK_CMD_TYPE::invalid )
                    {
                        wxLogDebug( "Invalid command type: %s", cmd_str.c_str() );
                        response = json{ { "error", "Invalid command type" }, { "cmd", cmd_str } }.dump();
                        request_server->Reply( response );
                        return;
                    }

                    switch( cmd )
                    {
                    case SDK_CMD_TYPE::netlist:
                    {
                        if( _handles.global_context_handle.expired() )
                        {
                            wxLogDebug( "Get design global context handle expired" );
                            response = json{ { "error", "Global context handle expired" } }.dump();
                        }
                        else
                        {
                            auto  context_function = _handles.global_context_handle.lock();
                            auto& global_ctx = ( *context_function )();
                            response = global_ctx.dump();
                        }
                        break;
                    }
                    case SDK_CMD_TYPE::placeNetLabels:
                    {
                        // Parse the full command with params
                        auto place_cmd = request.get<SDK_AGENT_ACTION>();
                        
                        if( _evtSink && !_handles.agent_action_handle.expired() )
                        {
                            _evtSink->CallAfter(
                                    [=]
                                    {
                                        auto context_function = _handles.agent_action_handle.lock();
                                        if( context_function )
                                        {
                                            ( *context_function )( place_cmd.params );
                                        }
                                    } );
                            response = json{ { "status", "ok" } }.dump();
                        }
                        else
                        {
                            response = json{ { "error", "Invalid event sink or agent handle" } }.dump();
                        }
                        break;
                    }
                    default:
                    {
                        response = json{ { "error", "Unknown command" }, { "cmd", cmd_str } }.dump();
                        break;
                    }
                    }

                    request_server->Reply( response );
                }
                catch( std::exception& e )
                {
                    json error_response = { { "error", e.what() } };
                    request_server->Reply( error_response.dump() );
                }
                catch( ... )
                {
                    json error_response = { { "error", "Unknown exception" } };
                    request_server->Reply( error_response.dump() );
                }
            } );

    // Keep the thread running until stopped
    while( !m_shouldStop )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    }
}
