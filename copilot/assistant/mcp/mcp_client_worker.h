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

#ifndef MCP_CLIENT_WORKER_H
#define MCP_CLIENT_WORKER_H

#include <atomic>
#include <string>
#include <thread>
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <nlohmann/json.hpp>


typedef wxMessageQueue<std::string> MCP_CLIENT_REQUEST_QUEUE;
enum class MCP_CMD_TYPE;

class WEBVIEW_CONTAINER;
class MCP_CLIENT_WORKER
{
public:
    MCP_CLIENT_WORKER( std::string const& aPairUrl, WEBVIEW_CONTAINER& eventSink,
                       MCP_CLIENT_REQUEST_QUEUE& requestQueue, std::string const& aSdkURL );

    ~MCP_CLIENT_WORKER();

    static long launch_mcp_py_client( std::string const& url, std::string const& sdkServerAddress );

    void Stop();

private:
    void Run( std::string const& aPairUrl );


private:
    std::string               m_request;
    nlohmann::json            m_mcp_settings{};
    WEBVIEW_CONTAINER&        m_eventSink;
    MCP_CLIENT_REQUEST_QUEUE& m_requestQueue;
    std::atomic_bool          m_shouldStop{ false };
    long                      m_pyMcpClientPid{};
    std::thread               m_thread;
};

#endif
