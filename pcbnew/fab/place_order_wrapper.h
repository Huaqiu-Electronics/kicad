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

#ifndef PLACE_ORDER_WRAPPER_H
#define PLACE_ORDER_WRAPPER_H

#include "place_order_utils.h"
#include <optional>
#include <thread_pool.h>

#include <quote/pcb_smt_form.h>
#include <utils/file_path_generator.h>

#include <settings/webview_settings_manager.h>
#include <auth/dialog_login.h>
#include <wx/utils.h>


template <>
void FAB_INFO_GETTER_PCB::PlaceOrder( ORDER_TYPE aOrderType )
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

    m_progress_reporter =
            std::make_unique<WX_PROGRESS_REPORTER>( self(), _( "Preparing Order" ), aOrderType == PCB ? 4 : 6 );
    std::shared_ptr<nullptr_t> on_leave{ nullptr, [this]( nullptr_t )
                                         {
                                             m_progress_reporter.reset();
                                         } };


    thread_pool& tp = GetKiCadThreadPool();
    auto         ret = tp.submit(
            [=]() -> std::optional<std::string>
            {
                switch( aOrderType )
                {
                case PCB: return self()->PlaceOrderForPCB();
                case SMT: return self()->PlaceOrderForSMT();
                }

                return {};
            } );

    std::future_status status = ret.wait_for( std::chrono::milliseconds( 250 ) );

    while( status != std::future_status::ready )
    {
        if( m_progress_reporter )
            m_progress_reporter->KeepRefreshing();

        status = ret.wait_for( std::chrono::milliseconds( 250 ) );
    }

    try
    {
        auto res = ret.get();

        if( !res )
            return;

        LaunchPlaceOrderUrl( *res );
    }
    catch( const std::exception& e )
    {
        wxMessageBox( wxString::FromUTF8( e.what() ), _( "Error" ), wxOK | wxICON_ERROR );
    }
}

#endif
