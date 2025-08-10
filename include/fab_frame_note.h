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

#ifndef FAB_FRAME_NOTE_H
#define FAB_FRAME_NOTE_H

#include <wx/anybutton.h>
#include <widgets/bitmap_button.h>
#include <auth/dialog_note.h>
#include <wx/frame.h>
#include <bitmaps.h>

template <class EditorT>
class FAB_FRAME_NOTE
{
public:
    auto self() { return static_cast<EditorT*>( this ); }
    auto self() const { return static_cast<const EditorT*>( this ); }

    void CreateNoteButton()
    {
        self()->Show(true);
        wxStatusBar*  statusBar = self()->GetStatusBar();
        if(statusBar == nullptr) return;
        m_notification = new BITMAP_BUTTON(statusBar, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        if(m_notification == nullptr) return;
        m_notification->SetPadding( 0 );
        m_notification->SetBitmap( KiBitmapBundle( BITMAPS::notifications ) );
        m_notification->SetShowBadge( true );
        m_notification->SetBitmapCentered( true );
        m_notification->Bind( wxEVT_LEFT_DOWN, &FAB_FRAME_NOTE<EditorT>::OnNaticateIconClick, self() );

        MoveNoteButton();
    }

    void MoveNoteButton()
    {
        wxStatusBar* statusBar = self()->GetStatusBar();
        if(statusBar == nullptr) return;
        int lastFiled = statusBar->GetFieldsCount() - 1;
        wxRect r;
        statusBar->GetFieldRect( lastFiled , r );
        auto buttonSize = m_notification->GetEffectiveMinSize();
        int x = r.GetRight() - buttonSize.GetWidth() -20;
        int y = r.GetTop();
        m_notification->SetPosition( { x, y } );
    }

    void OnNaticateIconClick(wxMouseEvent& aEvent)
    {
        int dlgWidth = 300;
        int dlgheight = 500;
        auto btnScreenPos = m_notification->GetScreenPosition();
        int dialogX = btnScreenPos.x + 40 - dlgWidth;
        int dialogY = btnScreenPos.y + 5 - dlgheight;
        if(m_promotedlg == nullptr)
        {
            m_promotedlg = new DIALOG_NOTE(self()); 
            m_promotedlg->Move(wxPoint(dialogX, dialogY));
            m_promotedlg->SetSize(wxSize( dlgWidth, dlgheight));
            m_promotedlg->LoadUrl("http://www.fdatasheets.com/news/page/bottomNotice/index.html");
            m_promotedlg->SetVisible(false);
        }
        m_promotedlg->SetVisible(!m_promotedlg->GetVisible());
        m_promotedlg->Show(m_promotedlg->GetVisible());
    }

private:
    BITMAP_BUTTON*  m_notification{};

    DIALOG_NOTE*    m_promotedlg{};
};



#endif