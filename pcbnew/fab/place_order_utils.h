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
    return wxString2String( wxFileName( board->GetFileName() ).GetName() );
}

inline std::string get_pcb_thickness( double aBrdThickness )
{
    static const std::vector<std::pair<std::string, double>> available_thicknesses = {
        { "0.6", 0.6 }, { "0.8", 0.8 }, { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 },
        { "2.0", 2.0 }, { "2.5", 2.5 }, { "3.0", 3.0 }, { "3.2", 3.2 }
    };

    for( const auto& thickness : available_thicknesses )
    {
        if( thickness.second >= aBrdThickness )
        {
            return thickness.first;
        }
    }

    return available_thicknesses.back().first;
}

enum class CopperThicknessType
{

    INNER,
    OUTER
};

inline std::string get_copper_thickness( double aBrdThickness, CopperThicknessType aType )
{
    static const std::vector<std::pair<std::string, double>> available_inner_thicknesses = {
        { "0.5", 0.5 }, { "1", 1.0 }, { "2", 2.0 }, { "3", 3.0 }, { "4", 4.0 }
    };

    static const std::vector<std::pair<std::string, double>> available_outer_thicknesses = {
        { "0.5", 0.5 }, { "1", 1.0 }, { "2", 2.0 }, { "3", 3.0 }, { "4", 4.0 }
    };

    const auto& available_thicknesses =
            aType == CopperThicknessType::INNER ? available_inner_thicknesses : available_outer_thicknesses;

    const auto oz = aBrdThickness * 1000 / 34.7;

    for( const auto& t : available_thicknesses )
    {
        if( oz >= t.second )
        {
            return t.first;
        }
    }

    return available_thicknesses.back().first;
}

inline std::string get_min_trace_clearance( double aTraceWidth )
{
    static const std::vector<std::pair<std::string, double>> available_trace_clearance = {
        { "3.5", 3.5 }, { "4", 4.0 }, { "5", 5.0 }, { "6", 6.0 }, { "8", 8.0 }, { "10", 10.0 }
    };

    if( aTraceWidth <= available_trace_clearance.front().second )
    {
        return available_trace_clearance.front().first;
    }

    if( aTraceWidth >= available_trace_clearance.back().second )
    {
        return available_trace_clearance.back().first;
    }

    for( size_t i = 0; i < available_trace_clearance.size() - 1; ++i )
    {
        double a = available_trace_clearance[i].second;
        double b = available_trace_clearance[i + 1].second;

        if( aTraceWidth > a && aTraceWidth <= b )
        {
            return available_trace_clearance[i].first;
        }
    }

    return available_trace_clearance[0].first;
}
inline std::string get_min_hole_size( double aHoleSize )
{
    static const std::vector<std::pair<std::string, double>> available_hole_size = {
        { "0.15", 0.15 }, { "0.2", 0.2 }, { "0.25", 0.25 }, { "0.3", 0.3 }
    };

    if( aHoleSize <= available_hole_size.front().second )
    {
        return available_hole_size.front().first;
    }

    if( aHoleSize >= available_hole_size.back().second )
    {
        return available_hole_size.back().first;
    }

    for( size_t i = 0; i < available_hole_size.size() - 1; ++i )
    {
        double a = available_hole_size[i].second;
        double b = available_hole_size[i + 1].second;

        if( aHoleSize > a && aHoleSize <= b )
        {
            return available_hole_size[i].first;
        }
    }

    return available_hole_size[0].first;
}


#endif
