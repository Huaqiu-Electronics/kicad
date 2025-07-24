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
#include <wx/string.h>


template <>
void FAB_INFO_GETTER_PCB::PlaceOrder()
{
    if( !m_placeOrderDialog )
    {
        m_placeOrderDialog = new DIALOG_WEBVIEW( self(), wxID_ANY, _( "Place Order" ), "HQ_PCB" );
        m_placeOrderDialog->LoadUrl(
                "https://www.hqpcb.com/mycenter/?token=695fb224-f5f5-5f8f-9a28-c0ede7f56f9e-6874cdbb" );
    }

    m_placeOrderDialog->Show();
}

#endif
