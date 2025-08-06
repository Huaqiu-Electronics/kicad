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

#ifndef WEBVIEW_TOKEN_CONTROLLER_H
#define WEBVIEW_TOKEN_CONTROLLER_H

#include <copilot_global.h>
#include <set>
#include "webview_msg_handle.h"

class WEBVIEW_CONTAINER;
class COPILOT_API WEBVIEW_TOKEN_CONTROLLER
{
public:
    ~WEBVIEW_TOKEN_CONTROLLER();

    static WEBVIEW_TOKEN_CONTROLLER& Get();

    void OnPageLoaded( WEBVIEW_CONTAINER* aWebviewContainer );

    void OnTokenUpdated();

    void OnLogout();

    void OnLoginFailed( WEBVIEW_CONTAINER* aWebviewContainer );

    void AddWebviewContainer( WEBVIEW_CONTAINER* aWebviewContainer );

    void RemoveWebviewContainer( WEBVIEW_CONTAINER* aWebviewContainer );

private:
    WEBVIEW_TOKEN_CONTROLLER();

    void ExpireToken( WEBVIEW_CONTAINER* aWebviewContainer );

    void UpdateToken( WEBVIEW_CONTAINER* aWebviewContainer );

    void Logout( WEBVIEW_CONTAINER* aWebviewContainer );

    WEBVIEW_MSG_HANDLE_T         _msg_handle;
    std::set<WEBVIEW_CONTAINER*> _webview_container_set;
};



#endif
