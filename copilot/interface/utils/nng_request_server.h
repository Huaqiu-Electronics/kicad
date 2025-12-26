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

#ifndef NNG_REQUEST_SERVER_H
#define NNG_REQUEST_SERVER_H

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <copilot_global.h>

class COPILOT_API NNG_REQUEST_SERVER
{
public:
    NNG_REQUEST_SERVER( const std::string& aSocketUrl );
    ~NNG_REQUEST_SERVER();


    bool Start();

    void Stop();

    bool Running() const;

    void SetCallback( std::function<void( std::string* )> aFunc ) { m_callback = aFunc; }

    void Reply( const std::string& aReply );

    const std::string& SocketPath() const { return m_socketUrl; }

private:
    void listenThread();

    std::thread m_thread;

    std::atomic<bool> m_shutdown;

    std::string m_socketUrl;

    std::function<void( std::string* )> m_callback;

    std::string m_sharedMessage;

    std::string m_pendingReply;

    std::condition_variable m_replyReady;

    std::mutex m_mutex;
};


#endif
