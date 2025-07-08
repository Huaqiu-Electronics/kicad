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

#include "dialog_webview.h"
#include <settings/dialog_geometry.h>

#include <settings/webview_settings_manager.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <fmt/format.h>

DIALOG_WEBVIEW::DIALOG_WEBVIEW( wxWindow* aParent, wxWindowID id, const wxString& title, const wxString& settingKey ) :
        wxDialog( aParent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxRESIZE_BORDER,
                  settingKey ),
        _browser( wxWebView::New() ), _settingKey( settingKey )
{
    const auto geometry = WEBVIEW_SETTINGS_MANAGER::get_instance().get_dialog_geometry( _settingKey.ToStdString() );
    SetPosition( { geometry.x, geometry.y } );
    SetSize( { geometry.width, geometry.height } );

    auto top_sizer = new wxBoxSizer( wxVERTICAL );
    _browser->Create( this, wxID_ANY );
    top_sizer->Add( _browser, wxSizerFlags().Expand().Proportion( 1 ) );
    SetSizer( top_sizer );

    Bind( wxEVT_CLOSE_WINDOW,
          [this]( wxCloseEvent& event )
          {
              const auto pos = GetPosition();
              const auto size = GetSize();
              WEBVIEW_SETTINGS_MANAGER::get_instance().set_dialog_viewer_geometry(
                      _settingKey.ToStdString(), DIALOG_GEOMETRY{ pos.x, pos.y, size.x, size.y

                                                 } );
              event.Skip();
          } );


#ifdef DEBUG

    _browser->EnableAccessToDevTools( true );

#endif // DEBUG
}

DIALOG_WEBVIEW::~DIALOG_WEBVIEW()
{
}

void DIALOG_WEBVIEW::LoadUrl( wxString const& aUrl )
{
    _browser->LoadURL( aUrl );
}

wxWebView& DIALOG_WEBVIEW::GetBrowser()
{
    return *_browser;
}
