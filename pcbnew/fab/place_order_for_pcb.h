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
#include <memory>
#include <pcb_edit_frame.h>
#include <board.h>
#include <string>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <auth/dialog_login.h>
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include <quote/pcb_form.h>
#include <quote/quote_controller.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <utils/copilot_zip_utils.h>


template <>
std::optional<std::string> FAB_INFO_GETTER_PCB::PlaceOrderForPCB()
{
    auto zipFilePath = GetPCBFileZip();

    if( !zipFilePath )
    {
        return {};
    }

    ReportProgress( _( "Sending Order" ) );
    QUOTE_CONTROLLER controller;
    auto             rsp = controller.pcb_quote( *GetPCB_FORM(), zipFilePath->ToStdString() );

    if( !rsp.has_value() || rsp->id.empty() )
    {
        ReportProgress( _( "Failed to send order" ) );
        return {};
    }

    constexpr auto kOrderFromDFM = "354";

    return fmt::format( "https://www.hqpcb.com/mycenter/orders/pcb/quote?type=PCB&id={}&guid={}&tid={}&token={}",
                        rsp->id, rsp->guid, kOrderFromDFM,
                        WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token );
}


#endif
