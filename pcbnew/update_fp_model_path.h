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

#ifndef UPDATE_FP_MODEL_PATH_H
#define UPDATE_FP_MODEL_PATH_H


#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <pcb_io/pcb_io_mgr.h>
#include <footprint.h>
#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <wx/log.h>
#include <io/kicad/kicad_io_utils.h>

static bool UpdateFpModelPath( wxString const& fpFilePath, wxString const& modelPath )
{
    PCB_IO_KICAD_SEXPR pi;


    if( pi.CanReadFootprint( fpFilePath ) )
    {
        wxString footprintNameOut;

        auto footprint = pi.ImportFootprint( fpFilePath, footprintNameOut );

        if( !footprint )
            return {};

        footprint->Models().clear();
        const auto model = new FP_3DMODEL();
        model->m_Filename = modelPath;
        footprint->Add3DModel( model );

        pi.Format( footprint );

        FILE* fp = wxFopen( fpFilePath, wxT( "wt" ) );

        if( fp == nullptr )
        {
            wxLogError( wxString::Format( _( "Insufficient permissions to write file '%s'." ), fpFilePath ) );
            return {};
        }

        std::string prettyData = pi.GetStringOutput( false );
        KICAD_FORMAT::Prettify( prettyData, true );

        fprintf( fp, "%s", prettyData.c_str() );
        fclose( fp );

        delete footprint;
        return true;
    }

    return {};
}

#endif
