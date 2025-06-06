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

#ifndef COPILOT_CLIPBOARD_H
#define COPILOT_CLIPBOARD_H

#include <wx/clipbrd.h>
#include <wx/log.h>


inline auto WriteClipBoard( std::string const& aData )
{
    wxLogNull         doNotLog; // disable logging of failed clipboard actions
    auto              clipboard = wxTheClipboard;
    wxClipboardLocker clipboardLock( clipboard );

    if( !clipboardLock || !clipboard->IsOpened() )
        return;

    clipboard->SetData( new wxTextDataObject( aData ) );

    clipboard->Flush();

#ifndef __WXOSX__
    // This section exists to return the clipboard data, ensuring it has fully
    // been processed by the system clipboard.  This appears to be needed for
    // extremely large clipboard copies on asynchronous linux clipboard managers
    // such as KDE's Klipper. However, a read back of the data on OSX before the
    // clipboard is closed seems to cause an ASAN error (heap-buffer-overflow)
    // since it uses the cached version of the clipboard data and not the system
    // clipboard data.
    if( clipboard->IsSupported( wxDF_TEXT ) || clipboard->IsSupported( wxDF_UNICODETEXT ) )
    {
        wxTextDataObject data;
        clipboard->GetData( data );
        void( data.GetText() );
    }
#endif
}


#endif
