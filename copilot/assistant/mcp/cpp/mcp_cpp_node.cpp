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

#include "mcp_cpp_node.h"
#include <thread>
#include <string>
MCP_CPP_NODE::MCP_CPP_NODE()
{
}

MCP_CPP_NODE::~MCP_CPP_NODE()
{
}

bool MCP_CPP_NODE::Running() const
{
    return true;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nng/nng.h>
#include <nng/protocol/pair0/pair.h>


const char* node0_name = "node0";
const char* node1_name = "node1";

const auto DEFAULT_SETTINGS = R"(
)";


const auto DEFAULT_CMD = R"(
{
    "cmd_type": "complete",
    "msg": "截图https://www.eda.cn/ 的首页，并提取所有的标题",
    "server_names": ["markitdown", "playwright"]
}
    )";

void fatal( const char* func, int rv )
{
    fprintf( stderr, "%s: %s\n", func, nng_strerror( rv ) );
    exit( 1 );
}

int send_cmd( nng_socket sock, const char* name )
{
    nng_send( sock, (void*) name, strlen( name ), 0 );
    return 1;
}

int recv_result( nng_socket sock )
{
    char*  buf = NULL;
    int    rv;
    size_t sz;
    rv = nng_recv( sock, &buf, &sz, NNG_FLAG_ALLOC );
    if( rv == 0 && buf )
    {
        // Convert to std::string using size
        std::string str( buf, sz );

        // Print to console/log
        printf( "Received: %s\n", str.c_str() );

        // Append to D:/test.log
        FILE* fp = fopen( "D:/test.log", "a" );
        if( fp )
        {
            fwrite( str.c_str(), 1, str.size(), fp );
            fputc( '\n', fp );
            fclose( fp );
        }

        nng_free( buf, sz );
    }
    return rv;
}
int send_recv( nng_socket sock )
{
    int rv;
    if( ( rv = nng_setopt_ms( sock, NNG_OPT_RECVTIMEO, 100 ) ) != 0 )
    {
        fatal( "nng_setopt_ms", rv );
    }
    for( ;; )
    {
        send_cmd( sock, DEFAULT_SETTINGS );
        send_cmd( sock, DEFAULT_CMD );
        _sleep( 1 );
        recv_result( sock );
    }
}

int node0( const char* url )
{
    nng_socket sock;
    int        rv;
    if( ( rv = nng_pair0_open( &sock ) ) != 0 )
    {
        fatal( "nng_pair0_open", rv );
    }
    if( ( rv = nng_listen( sock, url, NULL, 0 ) ) != 0 )
    {
        fatal( "nng_listen", rv );
    }
    return ( send_recv( sock ) );
}


int run()
{
    node0( "ipc:///tmp/pair.ipc" );
    return 1;
}

void MCP_CPP_NODE::Run()
{
    std::thread t{ run };
    t.detach();
}
