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

#include "kicad_sdk_http_server.h"

#include "context/copilot_global_context.h"
#include <passive_action/agent/agent_action.h>
#include <utils/httplib_wrapper.h>
#include <nlohmann/json.hpp>
#include <utility>
#include <wx/log.h>
#include <wx/panel.h>


using json = nlohmann::json;
using namespace std;

struct KICAD_SDK_HTTP_SERVER_IMPL
{
    httplib::Server _server;
};

KICAD_SDK_HTTP_SERVER::KICAD_SDK_HTTP_SERVER( HOST_COPILOT_HANDLES aHandles, wxPanel* evtSink,
                                              std::unique_ptr<httplib::Server> aServer ) :
        _handles( std::move( aHandles ) ), _evtSink( evtSink ), _server( std::move( aServer ) )
{
}


KICAD_SDK_HTTP_SERVER::~KICAD_SDK_HTTP_SERVER()
{
    if( _server )
    {
        _server->stop();
    }
}


void KICAD_SDK_HTTP_SERVER::Start()
{
    _server_thread = std::thread( &KICAD_SDK_HTTP_SERVER::run_server, this );
    _server_thread.detach();
}


void KICAD_SDK_HTTP_SERVER::run_server()
{
    _server->Get( "/",
                  [&]( const httplib::Request&, httplib::Response& res )
                  {
                      res.set_content( "KiCad SDK HTTP Server is running", "text/plain" );
                  } );
    _server->Get( "/netlist",
                  [&]( const httplib::Request&, httplib::Response& res )
                  {
                      if( _handles.global_context_handle.expired() )
                      {
                          wxLogDebug( "Get design global context handle expired" );
                          return;
                      }

                      auto  context_function = _handles.global_context_handle.lock();
                      auto& global_ctx = ( *context_function )();
                      auto  global_ctx_value = global_ctx.dump();
                      res.set_content( global_ctx_value, "application/json" );
                  } );

    _server->Post( "/placeNetLabels",
                   [&]( const httplib::Request& req, httplib::Response& res )
                   {
                       try
                       {
                           auto params = json::parse( req.body );
                           _evtSink->CallAfter(
                                   [=]
                                   {
                                       if( !_handles.agent_action_handle.expired() )
                                       {
                                           auto context_function = _handles.agent_action_handle.lock();
                                           ( *context_function )( params );
                                       }
                                   } );

                           res.set_content( "{\"status\":\"ok\"}", "application/json" );
                       }
                       catch( std::exception& e )
                       {
                           res.status = 400;
                           res.set_content( "{\"error\":\"invalid json\"}", "application/json" );
                       }
                   } );

    _server->listen_after_bind();
}
