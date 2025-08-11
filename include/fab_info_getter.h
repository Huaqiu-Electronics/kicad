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

#ifndef FAB_INFO_GETTER_H
#define FAB_INFO_GETTER_H


#include <vector>
#include <tool/action_toolbar.h>
#include <tool/actions.h>
#include <tool/selection_tool.h>

#include <fab/fab_pcb_specifications.h>
#include <fab/fab_bom.h>
#include <fab/fab_footprint_position.h>


class DIALOG_WEBVIEW;
template <class EditorT>
class FAB_INFO_GETTER
{
public:
    auto self() { return static_cast<EditorT*>( this ); }
    auto self() const { return static_cast<const EditorT*>( this ); }

    // For PCB query price
    FAB_PCB_SPECIFICATIONS GetPCBSpecifications() { return {}; }

    bool GenerateGerberFiles( wxString const& aOutDir ) { return {}; }

    bool GenerateDrillFile( wxString const& aOutDir ) { return {}; }

    // For SMT
    std::vector<FP_POSITION_LIST> GetComponentPosition() { return {}; }

    // For components
    std::vector<FAB_BOM_ITEM> GetBOM() { return {}; }

    void PlaceOrderForPCB() {}

    void PlaceOrderForAll() {}

    void ToggleHQPersonalCenter() {}

    void RecreateFabToolBar()
    {
        auto aToolBar = self()->m_mainToolBar;
        aToolBar->AddScaledSeparator( self() );
        aToolBar->Add( ACTIONS::placeOrderMenu );
        auto placeOrderMenu =
                std::make_unique<ACTION_MENU>( false, self()->GetToolManager()->GetTool<SELECTION_TOOL>() );
        placeOrderMenu->Add( ACTIONS::placeOrderForAll );
        placeOrderMenu->Add( ACTIONS::placeOrderForPCB );
        aToolBar->AddToolContextMenu( ACTIONS::placeOrderMenu, std::move( placeOrderMenu ) );
        aToolBar->AddScaledSeparator( self() );
    }

    void TriggerPlaceOrderMenu()
    {
        auto              aToolBar = self()->m_mainToolBar;
        wxAuiToolBarEvent evt;
        evt.SetEventType( wxEVT_AUITOOLBAR_RIGHT_CLICK );
        evt.SetToolId( ACTIONS::placeOrderMenu.GetUIId() );
        aToolBar->HandleWindowEvent( evt );
    }
};


#endif
