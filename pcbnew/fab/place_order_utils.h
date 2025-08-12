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

#ifndef PLACE_ORDER_UTILS_H
#define PLACE_ORDER_UTILS_H

#include <settings/webview_settings_manager.h>
#include <pcb_edit_frame.h>
#include <board.h>
#include <string>
#include <common/dialog_webview.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <auth/dialog_login.h>
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include <quote/pcb_order.h>
#include <quote/quote_controller.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <utils/copilot_zip_utils.h>
#include <utils/wstring_to_string.h>


template <>
FAB_PCB_SPECIFICATIONS FAB_INFO_GETTER_PCB::GetPCBSpecifications();

template <>
bool FAB_INFO_GETTER_PCB::GenerateDrillFile( wxString const& aOutDir );

template <>
bool FAB_INFO_GETTER_PCB::GenerateGerberFiles( wxString const& aOutDir );

template <>
std::optional<wxString> FAB_INFO_GETTER_PCB::GetPCBFileZip();

template <>
std::unique_ptr<PCB_ORDER> FAB_INFO_GETTER_PCB::GetPCB_ORDER();

template <>
std::unique_ptr<SMT_ORDER> FAB_INFO_GETTER_PCB::GetSTM_ORDER();

template <>
std::vector<FP_POSITION> FAB_INFO_GETTER_PCB::GetComponentPosition();

template <>
std::vector<FAB_BOM_ITEM> FAB_INFO_GETTER_PCB::GetBOM();

inline auto get_board_filename( BOARD* board )
{
    return ws2s( wxFileName( board->GetFileName() ).GetName().ToStdWstring() );
}

inline std::string get_pcb_thickness( int layer_count )
{
    switch( layer_count )
    {
    case 1:
    case 2:
    case 4: return "0.6";
    case 6: return "0.8";
    case 8: return "1.0";
    case 10: return "1.2";
    case 12: return "1.6";
    case 14: return "1.6";
    case 16: return "2.0";
    case 18: return "2.0";
    case 20: return "2.0";
    default: return "1.6";
    }
}

inline std::string get_min_trace_clearance( double aTraceWidth )
{
    // Corresponds to MIN_TRACE_WIDTH_CLEARANCE_CHOICE = [10, 8, 6, 5, 4, 3.5] (unit: mil)
    // Return the first element >= aTraceWidth, else the smallest
    static const double choices[] = { 10, 8, 6, 5, 4, 3.5 };
    for( double v : choices )
    {
        if( aTraceWidth >= v )
            return std::to_string( v );
    }
    // If smaller than all, return the smallest
    return std::to_string( choices[sizeof( choices ) / sizeof( choices[0] ) - 1] );
}

inline std::string get_min_hole_size( double aHoleSize )
{
    // Corresponds to MIN_HOLE_SIZE_CHOICE = [0.3, 0.25, 0.2, 0.15] (unit: mm)
    // Return the first element <= aHoleSize, else the largest
    static const double choices[] = { 0.3, 0.25, 0.2, 0.15 };
    for( double v : choices )
    {
        if( aHoleSize >= v )
            return std::to_string( v );
    }
    // If smaller than all, return the smallest
    return std::to_string( choices[sizeof( choices ) / sizeof( choices[0] ) - 1] );
}


#endif
