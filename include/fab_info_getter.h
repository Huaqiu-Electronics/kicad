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


#include <common/dialog_webview.h>
#include <optional>
#include <vector>
#include <tool/action_toolbar.h>
#include <tool/actions.h>
#include <tool/action_menu.h>

#include <fab/fab_pcb_specifications.h>
#include <fab/fab_bom.h>
#include <fab/fab_footprint_position.h>
#include <memory>
#include <wx/string.h>
#include <wx/utils.h>
#include <widgets/progress_reporter_base.h>
#include <wx/log.h>

#include "place_order_action_container.h"

struct PCB_FORM;
struct SMT_FORM;

class DIALOG_WEBVIEW;
template <class EDITOR_T>
class FAB_INFO_GETTER : public PLACE_ORDER_ACTION_CONTAINER
{
public:
    enum PLACE_ORDER_DIALOG_SIZE
    {
        WIDTH = 1500,
        HEIGHT = 900
    };


    auto self() { return static_cast<EDITOR_T*>( this ); }
    auto self() const { return static_cast<const EDITOR_T*>( this ); }

    // For PCB query price
    FAB_PCB_SPECIFICATIONS GetPCBSpecifications() { return {}; }

    bool GenerateGerberFiles( wxString const& aOutDir )
    {
        WXUNUSED( aOutDir )
        return {};
    }

    bool GenerateDrillFile( wxString const& aOutDir )
    {
        WXUNUSED( aOutDir )
        return {};
    }

    // For SMT
    std::vector<FP_POSITION> GetComponentPosition() { return {}; }

    // For components
    std::vector<FAB_BOM_ITEM> GetBOM() { return {}; }

    std::optional<std::string> PlaceOrderForPCB() { return {}; }

    std::optional<std::string> PlaceOrderForSMT() { return {}; }

    void PlaceOrder( ORDER_TYPE aOrderType ) override { WXUNUSED( aOrderType ) }

    void ToggleHQPersonalCenter() {}

    void RecreateFabToolBar( TOOL_INTERACTIVE* tool )
    {
        auto aToolBar = self()->m_mainToolBar;
        aToolBar->AddScaledSeparator( self() );
        aToolBar->Add( ACTIONS::placeOrderMenu );
        auto placeOrderMenu = std::make_unique<ACTION_MENU>( false, tool );
        placeOrderMenu->Add( ACTIONS::placeOrderForPCB );
        placeOrderMenu->Add( ACTIONS::placeOrderForSMT );
        aToolBar->AddToolContextMenu( ACTIONS::placeOrderMenu, std::move( placeOrderMenu ) );
    }

    void TriggerPlaceOrderMenu() override
    {
        ACTION_TOOLBAR*   aToolBar = self()->m_mainToolBar;
        wxAuiToolBarEvent evt;
        evt.SetEventType( wxEVT_AUITOOLBAR_RIGHT_CLICK );
        evt.SetToolId( ACTIONS::placeOrderMenu.GetUIId() );
        aToolBar->Command( evt );
    }

    void ReportProgress( wxString const& aMessage )
    {
        if( !m_progress_reporter )
        {
            return;
        }

        m_progress_reporter->AdvancePhase( aMessage );
        m_progress_reporter->KeepRefreshing();
    }

    std::optional<wxString> GetPCBFileZip() { return {}; }

    std::unique_ptr<PCB_FORM> GetPCB_FORM() { return {}; }

    std::unique_ptr<SMT_FORM> GetSTM_FORM() { return {}; }

    void LaunchPlaceOrderUrl( std::string const& aUrl )
    {
        wxLogDebug( "Launching place order url : %s", aUrl );
        if( m_progress_reporter )
        {
            m_progress_reporter.reset();
        }

        DIALOG_WEBVIEW place_order_dialog( self(), _( "Place Order" ), "HQ_PCB",
                                           wxSize( PLACE_ORDER_DIALOG_SIZE::WIDTH, PLACE_ORDER_DIALOG_SIZE::HEIGHT ) );

        place_order_dialog.LoadUrl( aUrl );
        place_order_dialog.ShowModal();
    }


protected:
    std::unique_ptr<PROGRESS_REPORTER_BASE> m_progress_reporter;
};


#endif
