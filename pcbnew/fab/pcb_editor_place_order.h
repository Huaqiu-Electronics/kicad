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

    // Post the form for placing order and get the order ID


    if( !m_placeOrderDialog )
    {
        m_placeOrderDialog = new DIALOG_WEBVIEW( self(), wxID_ANY, _( "Place Order" ), "HQ_PCB", wxSize( 1500, 900 ) );
    }

    // https://www.hqpcb.com/mycenter/orders/pcb/quote?type=PCB&id=3747806&tid=354&token=XXXXXXXXXXXXXXXXXXX

    m_placeOrderDialog->LoadUrl( fmt::format( "https://www.hqpcb.com/mycenter/?{}",
                                              WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token()->token ) );
    m_placeOrderDialog->Show();
}

#endif
