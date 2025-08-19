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

#ifndef PLACE_ORDER_PROGRESS_REPORTER_H
#define PLACE_ORDER_PROGRESS_REPORTER_H

#include <wx/evtloop.h>
#include <widgets/progress_reporter_base.h>
#include <wx/progdlg.h>
#include <wx/gauge.h>
#include <wx/appprogress.h>

class PLACE_ORDER_PROGRESS_REPORTER : public PROGRESS_REPORTER_BASE, public wxProgressDialog
{
public:
    PLACE_ORDER_PROGRESS_REPORTER( wxWindow* aParent, const wxString& aTitle, int aNumPhases, bool aCanAbort = true,
                                   bool aReserveSpaceForMessage = true ) :
            PROGRESS_REPORTER_BASE( aNumPhases ),
            wxProgressDialog( aTitle, ( aReserveSpaceForMessage ? wxT( " " ) : wxT( "" ) ), 1, aParent,
                              // wxPD_APP_MODAL |   // Don't use; messes up OSX when called from
                              // quasi-modal dialog
                              wxPD_AUTO_HIDE | // *MUST* use; otherwise wxWidgets will spin
                                               // up another event loop on completion which
                                               // causes all sorts of grief
                                      ( aCanAbort ? wxPD_CAN_ABORT : 0 ) | wxPD_ELAPSED_TIME ),
            m_appProgressIndicator( aParent ), m_messageWidth( 0 )
    {
        m_appProgressIndicator.Pulse();
    }


    /**
     * Change the title displayed on the window caption.
     */
    void SetTitle( const wxString& aTitle ) override { wxProgressDialog::SetTitle( aTitle ); }

    bool KeepRefreshing( bool aWait = false ) override
    {
        if( aWait )
        {
            while( m_phase.load() < m_numPhases && m_numPhases > 0 )
            {
                if( !updateUI() )
                {
                    m_cancelled.store( true );
                    return false;
                }

                wxMilliSleep( 33 /* 30 FPS refresh rate */ );
            }

            return true;
        }
        else
        {
            return true;
        }
    }

private:
    bool updateUI() override
    {
        int cur = CurrentProgress();

        if( cur < 0 || cur > 1000 )
            cur = 0;

        SetRange( 1000 );

        wxString message;

        {
            std::lock_guard<std::mutex> guard( m_mutex );
            message = m_rptMessage;
        }


        if( m_messageChanged )
        {
            int newWidth = GetTextExtent( m_rptMessage ).x;

            if( newWidth > m_messageWidth )
            {
                m_messageWidth = newWidth;
                Fit();
            }

            m_messageChanged = false;
        }

        // Returns false when cancelled (if it's a cancellable dialog)
        bool diag = wxProgressDialog::Update( cur, message );

        return diag;
    }

private:
    wxAppProgressIndicator m_appProgressIndicator;
    int                    m_messageWidth;
};

#endif
