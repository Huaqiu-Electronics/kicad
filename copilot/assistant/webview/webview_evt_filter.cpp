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

#include "webview_evt_filter.h"
#include <wx/event.h>
#include <wx/webview.h>
WEBVIEW_EVT_FILTER::WEBVIEW_EVT_FILTER()
{
    wxEvtHandler::AddFilter( this );
}

WEBVIEW_EVT_FILTER::~WEBVIEW_EVT_FILTER()
{
    wxEvtHandler::RemoveFilter( this );
}

int WEBVIEW_EVT_FILTER::FilterEvent( wxEvent& event )
{
#ifdef _WIN32
    return Event_Skip;
#endif

    const auto evt_type = event.GetEventType();
    auto       obj = event.GetEventObject();


    if( evt_type == wxEVT_SET_FOCUS )
    {
        if( m_panels.find( obj ) == m_panels.end() )
        {
            for( auto& [panel, focused] : m_panel_focused )
                m_panel_focused[panel] = false;
        }
        else
        {
            m_panel_focused[obj] = true;
        }
    }

    if( evt_type == wxEVT_KILL_FOCUS && m_panels.find( obj ) != m_panels.end() )
    {
        m_panel_focused[obj] = false;
    }


    if( wxEVT_WEBVIEW_SCRIPT_RESULT == evt_type
        || wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED == evt_type )
    {
        if( auto it = m_panel_focused.find( obj ); it != m_panel_focused.end() && !it->second )
        {
            return Event_Ignore;
        }
    }


    return Event_Skip;
}


WEBVIEW_EVT_FILTER& WEBVIEW_EVT_FILTER::Get()
{
    static WEBVIEW_EVT_FILTER instance;
    return instance;
}

void WEBVIEW_EVT_FILTER::AddPanel( wxPanel* aPanel )
{
    m_panels.insert( aPanel );
    m_panel_focused[aPanel] = false;
}

void WEBVIEW_EVT_FILTER::RemovePanel( wxPanel* aPanel )
{
    m_panels.erase( aPanel );
    m_panel_focused.erase( aPanel );
}
