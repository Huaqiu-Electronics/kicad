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

#include "webview/copilot_webview_container.h"


COPILOT_WEBVIEW_CONTAINER::COPILOT_WEBVIEW_CONTAINER( wxWindow* parent, std::string url,
                                                      HOST_COPILOT_HANDLES host_copilot_handles,
                                                      std::unique_ptr<httplib::Server> msgServer ) :
        WEBVIEW_CONTAINER( parent, url, host_copilot_handles, std::move( msgServer ) )
{
    
}
COPILOT_WEBVIEW_CONTAINER::~COPILOT_WEBVIEW_CONTAINER()
{
}

void COPILOT_WEBVIEW_CONTAINER::OnDocumentLoaded( wxWebViewEvent& evt )
{
#ifndef _WIN32

    const auto evt_url = evt.GetURL().ToStdString();

    // NOTE: Adhoc, on *nux the webview is not loading the url correctly on the first time if url start with https
    if( evt_url == WEBVIEW_SETTINGS_MANAGER::get_instance().get_copilot_webview_url() )
    {
        _browser->LoadURL( _primary_url );
    }

#endif // !_WIN32
};