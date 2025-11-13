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
#include "kicad_sdk_http_server.h"
#include <utils/httplib_wrapper.h>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <wx/log.h>
#include <wx/panel.h>

#if defined( _WIN32 )
#include <windows.h>
#define LOCK_FILE_PATH std::filesystem::temp_directory_path().string() + "\\kicad_sdk_http_server.lock"
#else
#define LOCK_FILE_PATH "/tmp/kicad_sdk_http_server.lock"
#endif

using json = nlohmann::json;
using namespace std;

struct KICAD_SDK_HTTP_SERVER_IMPL
{
    httplib::Server svr;
};

KICAD_SDK_HTTP_SERVER& KICAD_SDK_HTTP_SERVER::Instance()
{
    static KICAD_SDK_HTTP_SERVER instance;
    return instance;
}

KICAD_SDK_HTTP_SERVER::~KICAD_SDK_HTTP_SERVER()
{
}

bool KICAD_SDK_HTTP_SERVER::check_singleton_lock()
{
    ifstream file( LOCK_FILE_PATH );
    if( !file.good() )
    {
        return false;
    }

    int pid;
    file >> pid;
    if( pid <= 0 )
    {
        return false;
    }

#if defined( _WIN32 )
    HANDLE hProcess = OpenProcess( SYNCHRONIZE, FALSE, pid );
    bool   alive = ( hProcess != nullptr );
    if( alive )
        CloseHandle( hProcess );
    return alive;
#else
    return ( kill( pid, 0 ) == 0 );
#endif
}

void KICAD_SDK_HTTP_SERVER::create_lock_file()
{
    ofstream file( LOCK_FILE_PATH, ios::trunc );
#if defined( _WIN32 )
    file << GetCurrentProcessId();
#else
    file << getpid();
#endif
}

void KICAD_SDK_HTTP_SERVER::remove_lock_file()
{
    std::error_code ec;
    std::filesystem::remove( LOCK_FILE_PATH, ec );
}

void KICAD_SDK_HTTP_SERVER::Start()
{
    if( check_singleton_lock() )
    {
        std::cout << "[KiCad SDK] Existing HTTP server found, skipping startup." << std::endl;
        return;
    }
    create_lock_file();
    server_thread = std::thread( &KICAD_SDK_HTTP_SERVER::run_server, this );
    server_thread.detach();
}

void KICAD_SDK_HTTP_SERVER::Stop()
{
    if( !impl )
        return;

    impl->svr.stop();
    remove_lock_file();
}

void KICAD_SDK_HTTP_SERVER::SetHandles( HOST_COPILOT_HANDLES const& aHandles, wxPanel* evtSink )
{
    handles = aHandles;
    _evtSink = evtSink;
}

KICAD_SDK_HTTP_SERVER::KICAD_SDK_HTTP_SERVER()
{
}

void KICAD_SDK_HTTP_SERVER::run_server()
{
    impl = std::make_unique<KICAD_SDK_HTTP_SERVER_IMPL>();
    auto& svr = impl->svr;

    svr.Get( "/netlist",
             [&]( const httplib::Request&, httplib::Response& res )
             {
                 if( handles.global_context_handle.expired() )
                 {
                     wxLogDebug( "Get design global context handle expired" );
                     return;
                 }

                 auto  context_function = handles.global_context_handle.lock();
                 auto& global_ctx = ( *context_function )();
                 auto  global_ctx_value = global_ctx.dump();
                 res.set_content( global_ctx_value, "application/json" );
             } );

    svr.Post( "/placeNetLabels",
              [&]( const httplib::Request& req, httplib::Response& res )
              {
                  try
                  {
                      auto params = json::parse( req.body );
                      _evtSink->CallAfter(
                              [=]
                              {
                                  if( !handles.agent_action_handle.expired() )
                                  {
                                      auto context_function = handles.agent_action_handle.lock();
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

    std::cout << "[KiCad SDK] HTTP server running at http://127.0.0.1:9234" << std::endl;
    svr.listen( "127.0.0.1", 9234 );
    remove_lock_file();
}
