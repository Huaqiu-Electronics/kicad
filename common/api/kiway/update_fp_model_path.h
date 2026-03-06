/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2026 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2026 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef UPDATE_FP_MODEL_PATH_H
#define UPDATE_FP_MODEL_PATH_H


#include <kiway.h>
#include <kiface_ids.h>
#include <wx/log.h>

using UPDATE_HANDLE_T = bool ( * )( const wxString& aFootprintPath, const wxString& aModelPath );

inline auto UpdateFpModelPath( KIWAY& aKiway, const wxString& aFootprintPath, const wxString& aModelPath )
{
    KIFACE* kiface = aKiway.KiFACE( KIWAY::FACE_PCB );

    if( !kiface )
    {
        wxLogError( " KIWAY::FACE_PCB not found" );
        return false;
    }

    const auto addr = kiface->IfaceOrAddress( KIFACE_ADDR_ID::KIFACE_UPDATE_FOOTPRINT_MODEL_PATH );

    if( !addr )
    {
        wxLogError( "KIFACE_UPDATE_FOOTPRINT_MODEL_PATH not found" );
        return false;
    }

    return reinterpret_cast<UPDATE_HANDLE_T>( addr )( aFootprintPath, aModelPath );
}


#endif
