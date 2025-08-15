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

#ifndef PLACE_ORDER_FORM_BUILDER_H
#define PLACE_ORDER_FORM_BUILDER_H

#include "place_order_utils.h"
#include "get_pcb_thickness.h"

#include <utils/file_path_generator.h>
#include <quote/smt_form.h>
#include <memory>
#include <wx/log.h>


template <>
std::optional<wxString> FAB_INFO_GETTER_PCB::GetPCBFileZip()
{
    auto tmpDir = FILE_PATH_GENERATOR::get().gen_tmp_dir();

    ReportProgress( _( "Generating Gerber files" ) );
    if( !GenerateGerberFiles( tmpDir ) )
    {
        ReportProgress( _( "Generate gerber failed" ) );
        return {};
    }


    ReportProgress( _( "Generating drill files" ) );
    if( !GenerateDrillFile( tmpDir ) )
    {
        ReportProgress( _( "Generate drill failed" ) );
        return {};
    }

    auto zipFilePath = tmpDir + get_board_filename( self()->GetBoard() ) + ".zip";

    ReportProgress( _( "Compression PCB files" ) );
    if( !COPILOT_ZIP_UTILS::zip( zipFilePath, tmpDir ) )
    {
        ReportProgress( _( "Compress PCB files failed" ) );
        return {};
    }

    FILE_PATH_GENERATOR::get().record_for_cleanup( zipFilePath );

    return wxString( zipFilePath );
}

inline auto to_fixed_double( double aValue, int aPrecision = 2 )
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision( aPrecision ) << aValue;
    return ss.str();
}


template <>
std::unique_ptr<PCB_FORM> FAB_INFO_GETTER_PCB::GetPCB_FORM()
{
    auto pcb_spec = GetPCBSpecifications();
    wxLogDebug( "PCB Specification: %s", nlohmann::json( pcb_spec ).dump() );

    const auto layer_count = pcb_spec.layer_count;
    auto       form = std::make_unique<PCB_FORM>();
    form->blayer = std::to_string( layer_count );

    // mm to cm
    form->bwidth = to_fixed_double( pcb_spec.width / 10 );
    form->blength = to_fixed_double( pcb_spec.length / 10 );
    form->bheight = get_pcb_thickness( layer_count, pcb_spec.board_thickness );


    form->lineweight = get_min_trace_clearance( pcb_spec.mini_trace_width * 39.3701 ); // mm to mil
    form->vias = get_min_hole_size( pcb_spec.mini_hole_size, pcb_spec.board_thickness );

    if( pcb_spec.outer_copper_thickness )
        form->copper = get_copper_thickness( *pcb_spec.outer_copper_thickness, CopperThicknessType::OUTER );

    if( pcb_spec.inner_copper_thickness )
        form->insidecopper = get_copper_thickness( *pcb_spec.inner_copper_thickness, CopperThicknessType::INNER );

    return form;
}

template <>
std::unique_ptr<SMT_FORM> FAB_INFO_GETTER_PCB::GetSTM_FORM()
{
    auto pcb_from = GetPCB_FORM();
    auto form = std::make_unique<SMT_FORM>();
    return form;
}


#endif
