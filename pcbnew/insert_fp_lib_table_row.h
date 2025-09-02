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

#ifndef INSERT_FP_LIB_TABLE_ROW_H
#define INSERT_FP_LIB_TABLE_ROW_H


#include <fp_lib_table.h>


static bool InsertFpLibTableRow( FP_LIB_TABLE* fpT, const wxString& aNick, const wxString& aURI, const wxString& aType,
                                 const wxString& fpTablePath, const wxString& aOptions = wxEmptyString,
                                 const wxString& aDescr = wxEmptyString )
{
    if( fpT->HasLibraryWithPath( aURI ) )
        return true;

    if( fpT->InsertRow( new FP_LIB_TABLE_ROW( aNick, aURI, aType, aOptions, aDescr ) ) )
    {
        fpT->Save( fpTablePath );
        return true;
    }
    return false;
}

#endif
