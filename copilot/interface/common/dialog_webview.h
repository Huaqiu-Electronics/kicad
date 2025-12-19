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

#ifndef DIALOG_WEBVIEW_H
#define DIALOG_WEBVIEW_H


#include <wx/dialog.h>
#include <wx/webview.h>
#include <copilot_global.h>
#include "copilot_dialog.h"
#include <host_copilot_handles.h>

class WEBVIEW_CONTAINER;
class wxPanel;
class COPILOT_API DIALOG_WEBVIEW : public COPILOT_DIALOG
{
    using COPILOT_DIALOG::Init;
    WEBVIEW_CONTAINER* _webview_container{};
    WEBVIEW_CONTAINER* _parent_webview{};

public:
    DIALOG_WEBVIEW( struct HOST_VERSION_INFO const& host_version_info, wxWindow* aParent, const wxString& title,
                    const wxString& settingKey, const wxSize& size = { 800, 600 }, const wxPoint& pt = wxDefaultPosition, wxWindowID id = wxID_ANY,
                    HOST_COPILOT_HANDLES host_copilot_handles = {}, WEBVIEW_CONTAINER* parent_webview = nullptr );
    ~DIALOG_WEBVIEW();


    void LoadUrl( wxString const& aUrl );

    void RunScriptAsync( wxString const& aScript );

    WEBVIEW_CONTAINER* GetWebviewContainer() const;

    WEBVIEW_CONTAINER* GetParentWebView() const { return _parent_webview; }

    auto SetParentWebView( WEBVIEW_CONTAINER* parent ) { _parent_webview = parent; }
};


#endif
