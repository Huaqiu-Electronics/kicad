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
#include <utils/file_path_generator.h>
#include <quote/smt_order.h>
#include <memory>


template <>
std::optional<wxString> FAB_INFO_GETTER_PCB::GetPCBFileZip()
{
    auto tmpDir = FILE_PATH_GENERATOR::get().gen_tmp_dir();

    if( !GenerateGerberFiles( tmpDir ) )
    {
        wxMessageBox( _( "Generate gerber failed" ), _( "Place Order" ), wxOK | wxICON_ERROR );
        return {};
    }


    if( !GenerateDrillFile( tmpDir ) )
    {
        wxMessageBox( _( "Generate drill failed" ), _( "Place Order" ), wxOK | wxICON_ERROR );
        return {};
    }

    auto zipFilePath = tmpDir + ".zip";

    if( !COPILOT_ZIP_UTILS::zip( zipFilePath, tmpDir ) )
    {
        wxMessageBox( _( "Compress PCB files failed" ), _( "Place Order" ), wxOK | wxICON_ERROR );
        return {};
    }

    FILE_PATH_GENERATOR::get().record_for_cleanup( zipFilePath );

    return wxString( zipFilePath );
}

template <>
std::unique_ptr<PCB_ORDER> FAB_INFO_GETTER_PCB::GetPCB_ORDER()
{
    auto form = std::make_unique<PCB_ORDER>();

    auto pcb_spec = GetPCBSpecifications();
    form->blayer = std::to_string( pcb_spec.layer_count );

    std::stringstream ss;
    ss << std::fixed << std::setprecision( 2 ) << pcb_spec.width / 10;
    form->bwidth = ss.str();

    ss.str( "" );
    ss.clear();
    ss << std::fixed << std::setprecision( 2 ) << pcb_spec.length / 10;
    form->blength = ss.str();

    form->bheight = get_pcb_thickness( pcb_spec.layer_count );

    form->lineweight =
            get_min_trace_clearance( std::min( pcb_spec.mini_trace_clearance, pcb_spec.mini_trace_width ) * 39.3701 );
    form->vias = get_min_hole_size( pcb_spec.mini_hole_size );
    return form;
}

template <>
std::unique_ptr<SMT_ORDER> FAB_INFO_GETTER_PCB::GetSTM_ORDER()
{
    auto pcb_from = GetPCB_ORDER();
    auto form = std::make_unique<SMT_ORDER>();
    form->pcb_ban_height = pcb_from->bheight;
    form->pcb_ban_width = pcb_from->blength;
    return form;
}


#endif
