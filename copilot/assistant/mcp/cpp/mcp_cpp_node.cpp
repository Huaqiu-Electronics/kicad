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

MCP_CPP_NODE::MCP_CPP_NODE()
{

}

MCP_CPP_NODE::~MCP_CPP_NODE()
{

}

bool MCP_CPP_NODE::Running() const
{
        
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nng/nng.h>
#include <nng/protocol/pair0/pair.h>

#define NODE0 "node0"
#define NODE1 "node1"

void
fatal(const char *func, int rv)
{
        fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
        exit(1);
}

int
send_name(nng_socket sock, char *name)
{
        int rv;
        printf("%s: SENDING \"%s\"\n", name, name);
        if ((rv = nng_send(sock, name, strlen(name) + 1, 0)) != 0) {
                fatal("nng_send", rv);
        }
        return (rv);
}

int
recv_name(nng_socket sock, char *name)
{
        char *buf = NULL;
        int rv;
        size_t sz;
        if ((rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC)) == 0) {
                printf("%s: RECEIVED \"%s\"\n", name, buf); 
                nng_free(buf, sz);
        }
        return (rv);
}

int
send_recv(nng_socket sock, char *name)
{
        int rv;
        if ((rv = nng_setopt_ms(sock, NNG_OPT_RECVTIMEO, 100)) != 0) {
                fatal("nng_setopt_ms", rv);
        }
        for (;;) {
                recv_name(sock, name);
                _sleep(1);
                send_name(sock, name);
        }
}

int
node0(const char *url)
{
        nng_socket sock;
        int rv;
        if ((rv = nng_pair0_open(&sock)) != 0) {
                fatal("nng_pair0_open", rv);
        }
         if ((rv = nng_listen(sock, url, NULL, 0)) !=0) {
                fatal("nng_listen", rv);
        }
        return (send_recv(sock, NODE0));
}

int
node1(const char *url)
{
        nng_socket sock;
        int rv;
        _sleep(1);
        if ((rv = nng_pair0_open(&sock)) != 0) {
                fatal("nng_pair0_open", rv);
        }
        if ((rv = nng_dial(sock, url, NULL, 0)) != 0) {
                fatal("nng_dial", rv);
        }
        return (send_recv(sock, NODE1));
}

int
run()
{
        node0("ipc:///tmp/pair.ipc");
        return 1;
}


void MCP_CPP_NODE::Run()
{
    run();
}
