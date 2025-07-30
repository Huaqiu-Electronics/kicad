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
#include <dialog_gen_footprint_position.h>
#include <dialog_plot.h>
#include <core/typeinfo.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pcb_edit_frame.h>
#include <board.h>
#include <board_design_settings.h>
#include <base_units.h>
#include <tools/pcb_actions.h>
#include <webview/dialog_webview.h>
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
#include <fstream>
#include <random>
#include <chrono>
#include <minizip/zip.h>
#include <minizip/ioapi.h>


template <>
void FAB_INFO_GETTER_PCB::PlaceOrder()
{
    // Ensure user is logged in before placing order
    if( !WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid() )
    {
        DIALOG_LOGIN login_dialog( self() );
        login_dialog.ShowModal();

        if( !WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid() )
        {
            wxMessageBox( _( "Please login to place order" ), _( "Place Order" ), wxOK | wxICON_ERROR );
            return;
        }
    }

    // 1. Generate a UUID for this order (using timestamp for simplicity)
    std::time_t now = std::time( nullptr );
    std::string uuid_str = std::to_string( now );

    // 2. Create a temp directory under system temp with the uuid
    wxString tmpDir = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + uuid_str;
    std::filesystem::create_directories( tmpDir.ToStdString() );

    // 3. Generate Gerber and Drill files into the temp dir
    this->GenerateGerberFiles( tmpDir );
    this->GenerateDrillFile( tmpDir );

    // 4. Zip the temp directory using minizip
    wxString zipFilePath = tmpDir + ".zip";
    {
        zipFile zf = zipOpen( zipFilePath.mb_str().data(), APPEND_STATUS_CREATE );
        if( zf )
        {
            for( const auto& entry : std::filesystem::directory_iterator( tmpDir.ToStdString() ) )
            {
                if( entry.is_regular_file() )
                {
                    std::ifstream     file( entry.path(), std::ios::binary );
                    std::vector<char> buffer( ( std::istreambuf_iterator<char>( file ) ),
                                              std::istreambuf_iterator<char>() );
                    zip_fileinfo      zi = {};
                    zipOpenNewFileInZip( zf, entry.path().filename().string().c_str(), &zi, nullptr, 0, nullptr, 0,
                                         nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION );
                    zipWriteInFileInZip( zf, buffer.data(), static_cast<unsigned int>( buffer.size() ) );
                    zipCloseFileInZip( zf );
                }
            }
            zipClose( zf, nullptr );
        }
    }

    PCB_ORDER form;

    QUOTE_CONTROLLER controller;
    auto             rsp = controller.pcb_quote( form, zipFilePath.ToStdString() );

    if( !rsp.has_value() || rsp->quote_id.empty() )
    {
        wxMessageBox( _( "Failed to place order" ), _( "Place Order" ), wxOK | wxICON_ERROR );
        return;
    }
    std::string order_id = rsp->quote_id;

    if( !m_placeOrderDialog )
    {
        m_placeOrderDialog = new DIALOG_WEBVIEW( self(), wxID_ANY, _( "Place Order" ), "HQ_PCB", wxSize( 1500, 900 ) );
    }

    m_placeOrderDialog->LoadUrl(
            fmt::format( "https://www.hqpcb.com/mycenter/orders/pcb/quote?type=PCB&id={}&tid=354&token={}", order_id,
                         WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token ) );

    m_placeOrderDialog->Show();
}


#endif