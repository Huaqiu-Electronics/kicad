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

#ifndef CREATE_FAB_TOOLBAR_H
#define CREATE_FAB_TOOLBAR_H


#include "tool/actions.h"
#include <tool/action_menu.h>
#include <tools/pcb_selection_tool.h>
#include <pcb_edit_frame.h>

template <>
void FAB_INFO_GETTER_PCB::RecreateFabToolBar()
{
    auto aToolBar = self()->m_mainToolBar;
    aToolBar->AddScaledSeparator( self() );
    aToolBar->Add( ACTIONS::placeOrderMenu );
    auto placeOrderMenu = std::make_unique<ACTION_MENU>( false, self()->GetToolManager()->GetTool<SELECTION_TOOL>() );
    placeOrderMenu->Add( ACTIONS::placeOrderForAll );
    placeOrderMenu->Add( ACTIONS::placeOrderForPCB );
    aToolBar->AddToolContextMenu( ACTIONS::placeOrderMenu, std::move( placeOrderMenu ) );
    aToolBar->AddScaledSeparator( self() );
}

template <>
void FAB_INFO_GETTER_PCB::TriggerPlaceOrderMenu()
{
    auto              aToolBar = self()->m_mainToolBar;
    wxAuiToolBarEvent evt;
    evt.SetEventType( wxEVT_AUITOOLBAR_RIGHT_CLICK );
    evt.SetToolId( ACTIONS::placeOrderMenu.GetUIId() );
    aToolBar->HandleWindowEvent( evt );
}


#endif
