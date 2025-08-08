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

#ifndef PLACE_ORDER_FOR_PCB_H
#define PLACE_ORDER_FOR_PCB_H

#include "place_order_utils.h"
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
void FAB_INFO_GETTER_PCB::PlaceOrderForPCB()
{
    // Ensure user is logged in before placing order
    if( !WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid() )
    {
        self()->EnsureMainstackForWebKit(
                [=]
                {
                    DIALOG_LOGIN login_dialog( self() );
                    login_dialog.ShowModal();

                    if( !login_dialog.Success() )
                    {
                        wxMessageBox( _( "Please login to place order" ), _( "Place Order" ), wxOK | wxICON_ERROR );
                        return;
                    }
                } );
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

    self()->EnsureMainstackForWebKit(
            [=]
            {
                DIALOG_WEBVIEW place_order_dialog( self(), _( "Place Order" ), "HQ_PCB", wxSize( 1500, 900 ) );
                place_order_dialog.LoadUrl( url );
                place_order_dialog.ShowModal();
            } );
}


#endif
