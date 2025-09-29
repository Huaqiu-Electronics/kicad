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

#ifndef MCP_CLIENT_THREAD_H
#define MCP_CLIENT_THREAD_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/msgqueue.h>

typedef wxMessageQueue<std::string> MCP_CLIENT_REQUEST_QUEUE;
enum MCP_CMD_TYPE;


class MCP_CLIENT_THREAD : public wxThread
{
public:
    MCP_CLIENT_THREAD( wxEvtHandler& eventSink, MCP_CLIENT_REQUEST_QUEUE& requestQueue );

    ~MCP_CLIENT_THREAD();

    ExitCode Entry() override;

private:
    wxEvtHandler&             m_eventSink;
    MCP_CLIENT_REQUEST_QUEUE& m_requestQueue;
};

#endif
