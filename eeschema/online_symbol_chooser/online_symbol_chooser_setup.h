/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2025 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software{} you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation{} either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY{} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program{} if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef ONLINE_SYMBOL_CHOOSER_SETUP_H
#define ONLINE_SYMBOL_CHOOSER_SETUP_H


#include <sch_edit_frame.h>
#include <tool/action_toolbar.h>
#include <sch_actions.h>
#include "symbol_chooser_webview_container.h"
#include "online_symbol_chooser_aui_settings.h"


void SCH_EDIT_FRAME::SymbolChooserInit()
{
    m_webSymbolChooser = new SYMBOL_CHOOSER_WEBVIEW_CONTAINER(
            this, eeconfig()->m_OnlineSymbolChooserSettings.url,
            { {}, m_copilotAgentActionHdl, {} } );
}


void SCH_EDIT_FRAME::InitSymbolChooserAui()
{
    if( m_webSymbolChooser )
    {
        m_containerAuiMgr.AddPane( m_webSymbolChooser,
                                   defaultOnlineSymbolChooserPanelInfo( this ) );
    }
}

void SCH_EDIT_FRAME::RecreateSymbolChooserToolBar()
{
    m_mainToolBar->AddScaledSeparator( this );
    m_mainToolBar->Add( SCH_ACTIONS::toggleOnlineSymbolChooser );
}


void SCH_EDIT_FRAME::SymbolChooserPanelShowChangedLanguage()
{
    if( m_webSymbolChooser )
    {
        wxAuiPaneInfo& web_panel_info = m_containerAuiMgr.GetPane( m_webSymbolChooser );
        bool           is_shown = web_panel_info.IsShown();
        web_panel_info.Caption( _( "Copilot" ) );
        web_panel_info.Show( is_shown );
    }
}

void SCH_EDIT_FRAME::ToggleSymbolChooserPanel()
{
    wxAuiPaneInfo& web_pane = m_containerAuiMgr.GetPane( CopilotPanelName() );
    ShowCopilot( !web_pane.IsShown() );
}

void SCH_EDIT_FRAME::ShowSymbolChooserPanel( bool show )
{
    auto& web_pane = m_containerAuiMgr.GetPane( CopilotPanelName() );

    bool now_shown = web_pane.IsShown();

    if( now_shown == show )
        return;

    web_pane.Show( show );

    if( web_pane.IsShown() )
    {
        if( web_pane.IsFloating() )
        {
            web_pane.FloatingSize( eeconfig()->m_OnlineSymbolChooserSettings.panel_float_width,
                                   eeconfig()->m_OnlineSymbolChooserSettings.panel_float_height );
            m_containerAuiMgr.Update();
        }
        else if( eeconfig()->m_OnlineSymbolChooserSettings.panel_docked_width > 0 )
        {
            // SetAuiPaneSize also updates m_auimgr
            SetAuiPaneSize( m_containerAuiMgr, web_pane,
                            eeconfig()->m_OnlineSymbolChooserSettings.panel_docked_width, -1 );
        }
    }
    else
    {
        if( web_pane.IsFloating() )
        {
            eeconfig()->m_OnlineSymbolChooserSettings.panel_float_width = web_pane.floating_size.x;
            eeconfig()->m_OnlineSymbolChooserSettings.panel_float_height = web_pane.floating_size.y;
        }
        else
        {
            eeconfig()->m_OnlineSymbolChooserSettings.panel_docked_width =
                    m_webSymbolChooser->GetSize().x;
        }

        m_containerAuiMgr.Update();
    }
}

void SCH_EDIT_FRAME::SaveSymbolChooserPanelCnf()
{
    if( m_webSymbolChooser )
    {
        auto& webPane = m_containerAuiMgr.GetPane( CopilotPanelName() );
        webPane.Show( eeconfig()->m_OnlineSymbolChooserSettings.panel_show );
        if( eeconfig()->m_OnlineSymbolChooserSettings.panel_show )
        {
            SetAuiPaneSize( m_containerAuiMgr, webPane,
                            eeconfig()->m_OnlineSymbolChooserSettings.panel_docked_width, -1 );
        }
    }
}

void SCH_EDIT_FRAME::LoadSymbolChooserPanelCnf()
{
    if( m_webSymbolChooser )
    {
        auto& webPane = m_auimgr.GetPane( CopilotPanelName() );
        eeconfig()->m_OnlineSymbolChooserSettings.panel_show = webPane.IsShown();

        if( webPane.IsDocked() )
            eeconfig()->m_OnlineSymbolChooserSettings.panel_docked_width =
                    m_webSymbolChooser->GetSize().x;
        else
        {
            eeconfig()->m_OnlineSymbolChooserSettings.panel_float_height = webPane.floating_size.y;
            eeconfig()->m_OnlineSymbolChooserSettings.panel_float_width = webPane.floating_size.x;
        }
    }
}


#endif
