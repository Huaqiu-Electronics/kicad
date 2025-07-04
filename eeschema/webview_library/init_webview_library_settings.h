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

#ifndef INIT_WEBVIEW_LIBRARY_SETTINGS_H
#define INIT_WEBVIEW_LIBRARY_SETTINGS_H

#include <eeschema_settings.h>
#include <settings/app_settings.h>
#include <settings/parameters.h>
#include "webview_library_settings.h"


#if !defined( IS_RC_BUILD )
constexpr auto kSymbolChooserUrl = "https://www.eda.cn/symbolSelector/";
#else
constexpr auto kSymbolChooserUrl = "http://www.fdatasheets.com/symbolSelector/";
#endif


void EESCHEMA_SETTINGS::InitWebviewLibrarySettings()
{
    m_params.emplace_back( new PARAM<bool>( "aui.webview_library_show", &m_WebViewLibrarySettings.panel_show, true ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.webview_library_docked_width", &m_WebViewLibrarySettings.panel_docked_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.webview_library_float_width", &m_WebViewLibrarySettings.panel_float_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.webview_library_float_height", &m_WebViewLibrarySettings.panel_float_height, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.webview_library_float_height", &m_WebViewLibrarySettings.panel_float_height, -1 ) );

    m_params.emplace_back(
            new PARAM<std::string>( "aui.webview_library_url", &m_WebViewLibrarySettings.url, kSymbolChooserUrl ) );
}

#endif
