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

#include <common/dialog_webview.h>
#include <settings/dialog_geometry.h>
#include <webview/webview_container.h>
#include <settings/webview_settings_manager.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <fmt/format.h>
#include <wx/display.h>

DIALOG_WEBVIEW::DIALOG_WEBVIEW( HOST_VERSION_INFO const& host_version_info, wxWindow* aParent, const wxString& title,
                                const wxString& settingKey, const wxSize& size, const wxPoint& pt, wxWindowID id,
                                HOST_COPILOT_HANDLES host_copilot_handles, WEBVIEW_CONTAINER* parent_webview ) :
        COPILOT_DIALOG( aParent, title, settingKey, size, pt, id ),
        _webview_container( new WEBVIEW_CONTAINER( host_version_info, this, {}, std::move( host_copilot_handles ) ) ),
        _parent_webview( parent_webview )
{
    Init( _webview_container );
}

DIALOG_WEBVIEW::~DIALOG_WEBVIEW()
{
}

void DIALOG_WEBVIEW::LoadUrl( wxString const& aUrl )
{
    _webview_container->LoadUrl( aUrl.ToStdString() );
}

void DIALOG_WEBVIEW::RunScriptAsync( wxString const& aScript )
{
    _webview_container->RunScriptAsync( aScript );
}

WEBVIEW_CONTAINER* DIALOG_WEBVIEW::GetWebviewContainer() const
{
    return _webview_container;
}
