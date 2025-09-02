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


#include <settings/app_settings.h>
#include <settings/parameters.h>
#include <webview_library/webview_library_settings.h>

void APP_SETTINGS_BASE::InitWebviewLibrarySettings()
{
    m_params.emplace_back( new PARAM<bool>( "webview_library.show", &m_WebViewLibrarySettings.panel_show, true ) );

    m_params.emplace_back(
            new PARAM<int>( "webview_library.docked_width", &m_WebViewLibrarySettings.panel_docked_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "webview_library.float_width", &m_WebViewLibrarySettings.panel_float_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "webview_library.float_height", &m_WebViewLibrarySettings.panel_float_height, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "webview_library.float_height", &m_WebViewLibrarySettings.panel_float_height, -1 ) );
}
