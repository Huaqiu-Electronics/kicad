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

#ifndef PCB_EDITOR_PLACE_ORDER_H
#define PCB_EDITOR_PLACE_ORDER_H

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
#include <filesystem>
#include <utils/copilot_zip_utils.h>
#include <iomanip>
#include <sstream>


template <>
FAB_PCB_SPECIFICATIONS FAB_INFO_GETTER_PCB::GetPCBSpecifications();

template <>
bool FAB_INFO_GETTER_PCB::GenerateDrillFile( wxString const& aOutDir );

template <>
bool FAB_INFO_GETTER_PCB::GenerateGerberFiles( wxString const& aOutDir );


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


template <>
void FAB_INFO_GETTER_PCB::PlaceOrderForPCB()
{
    // Ensure user is logged in before placing order
    if( !WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid() )
    {
        DIALOG_LOGIN login_dialog( self() );
        login_dialog.ShowModal();

        if( !login_dialog.Success() )
        {
            wxMessageBox( _( "Please login to place order" ), _( "Place Order" ), wxOK | wxICON_ERROR );
            return;
        }
    }

    std::time_t now = std::time( nullptr );
    std::string uuid_str = std::to_string( now );

    wxString tmpDir = wxFileName::GetTempDir() + wxFileName::GetPathSeparator()
                      + wxFileName( self()->GetBoard()->GetFileName() ).GetName() + "-" + uuid_str;
    std::filesystem::create_directories( tmpDir.ToStdString() );

    GenerateGerberFiles( tmpDir );
    GenerateDrillFile( tmpDir );

    wxString zipFilePath = tmpDir + ".zip";

    if( !COPILOT_ZIP_UTILS::zip( zipFilePath.ToStdString(), tmpDir.ToStdString() ) )
    {
        return;
    }


    PCB_ORDER form;

    auto pcb_spec = GetPCBSpecifications();
    form.blayer = std::to_string( pcb_spec.layer_count );

    std::stringstream ss;
    ss << std::fixed << std::setprecision( 2 ) << pcb_spec.width / 10;
    form.bwidth = ss.str();

    ss.str( "" );
    ss.clear();
    ss << std::fixed << std::setprecision( 2 ) << pcb_spec.length / 10;
    form.blength = ss.str();

    form.bheight = get_pcb_thickness( pcb_spec.layer_count );

    form.lineweight =
            get_min_trace_clearance( std::min( pcb_spec.mini_trace_clearance, pcb_spec.mini_trace_width ) * 39.3701 );
    form.vias = get_min_hole_size( pcb_spec.mini_hole_size );

    QUOTE_CONTROLLER controller;
    auto             rsp = controller.pcb_quote( form, zipFilePath.ToStdString() );

    if( !rsp.has_value() || rsp->id.empty() )
    {
        wxMessageBox( _( "Failed to place order" ), _( "Place Order" ), wxOK | wxICON_ERROR );
        return;
    }


    constexpr auto kOrderFromDFM = "354";

    const auto url = fmt::format(
            "https://www.hqpcb.com/mycenter/orders/pcb/quote?type=PCB&id={}&guid={}&tid={}&token={}", rsp->id,
            rsp->guid, kOrderFromDFM, WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token );


    DIALOG_WEBVIEW place_order_dialog( self(), _( "Place Order" ), "HQ_PCB", wxSize( 1500, 900 ) );
    place_order_dialog.LoadUrl( url );
    place_order_dialog.ShowModal();
}

template <>
void FAB_INFO_GETTER_PCB::PlaceOrderForAll()
{
}


#endif
