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
class COPILOT_API DIALOG_WEBVIEW : public COPILOT_DIALOG
{
    using COPILOT_DIALOG::Init;
    WEBVIEW_CONTAINER* _webview_container{};

public:
    DIALOG_WEBVIEW( wxWindow* aParent, wxWindowID id, const wxString& title, const wxString& settingKey,
                    const wxSize& size = wxDefaultSize, HOST_COPILOT_HANDLES host_copilot_handles = {} );
    ~DIALOG_WEBVIEW();


    void LoadUrl( wxString const& aUrl );

    void RunScript( wxString const& aScript );
};


#endif
