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

#include <tool/webview_evt_dispatcher.h>

#include <wx/app.h>
#include <wx/event.h>
#include <wx/webview.h>

inline auto isWebviewEvent( wxEventType evt_type )
{
    return evt_type == wxEVT_WEBVIEW_NAVIGATING || evt_type == wxEVT_WEBVIEW_NAVIGATED
           || evt_type == wxEVT_WEBVIEW_LOADED || evt_type == wxEVT_WEBVIEW_ERROR || evt_type == wxEVT_WEBVIEW_NEWWINDOW
           || evt_type == wxEVT_WEBVIEW_TITLE_CHANGED || evt_type == wxEVT_WEBVIEW_FULLSCREEN_CHANGED
           || evt_type == wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED || evt_type == wxEVT_WEBVIEW_SCRIPT_RESULT;
};

WEBVIEW_EVT_DISPATCHER::~WEBVIEW_EVT_DISPATCHER()
{
    wxEvtHandler::RemoveFilter( this );
}

int WEBVIEW_EVT_DISPATCHER::FilterEvent( wxEvent& event )
{
    if( m_panels.find( event.GetEventObject() ) == m_panels.end() )
    {
        return Event_Skip;
    }

    if( isWebviewEvent( event.GetEventType() ) )
    {
        m_events.push( event.Clone() );
        return Event_Ignore;
    }

    return Event_Skip;
}

void WEBVIEW_EVT_DISPATCHER::AddPanel( void* aPanel )
{
    m_panels.insert( aPanel );
}

void WEBVIEW_EVT_DISPATCHER::RemovePanel( void* aPanel )
{
    m_panels.erase( aPanel );
}

void WEBVIEW_EVT_DISPATCHER::FlushEvents()
{
    while( !m_events.empty() )
    {
        wxEvent* event = m_events.front();
        wxTheApp->ProcessEvent( *event );
        delete event;
        m_events.pop();
    }
}

WEBVIEW_EVT_DISPATCHER::WEBVIEW_EVT_DISPATCHER()
{
    wxEvtHandler::AddFilter( this );
}
