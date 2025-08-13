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

#ifndef PCB_FAB_CTRL_H
#define PCB_FAB_CTRL_H

#include "pcb_edit_frame.h"
#include "place_order_form_builder.h"
#include "place_order_for_pcb.h"
#include "place_order_for_all.h"
#include "place_order_wrapper.h"
#include "tools/pcb_control.h"


int PCB_CONTROL::TriggerPlaceOrderMenu( const TOOL_EVENT& aEvent )
{
    PCB_EDIT_FRAME* editFrame = dynamic_cast<PCB_EDIT_FRAME*>( m_frame );

    if( !editFrame )
        return 1;

    editFrame->TriggerPlaceOrderMenu();
    return 0;
}


int PCB_CONTROL::PlaceOrderForPCB( const TOOL_EVENT& aEvent )
{
    PCB_EDIT_FRAME* editFrame = dynamic_cast<PCB_EDIT_FRAME*>( m_frame );

    if( !editFrame )
        return 1;

    editFrame->PlaceOrder( PCB_EDIT_FRAME::ORDER_TYPE::PCB );
    return 0;
}

int PCB_CONTROL::PlaceOrderForAll( const TOOL_EVENT& aEvent )
{
    PCB_EDIT_FRAME* editFrame = dynamic_cast<PCB_EDIT_FRAME*>( m_frame );

    if( !editFrame )
        return 1;

    editFrame->PlaceOrder( PCB_EDIT_FRAME::ORDER_TYPE::ALL );
    return 0;
}


int PCB_CONTROL::ToggleHQPersonalCenter( const TOOL_EVENT& aEvent )
{
    PCB_EDIT_FRAME* editFrame = dynamic_cast<PCB_EDIT_FRAME*>( m_frame );

    if( !editFrame )
        return 1;

    editFrame->ToggleHQPersonalCenter();
    return 0;
}


#endif
