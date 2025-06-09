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

#ifndef COPILOT_PANEL_CONTAINER_H
#define COPILOT_PANEL_CONTAINER_H

#include <passive_action/agent/agent_action.h>
#include <magic_enum.hpp>
#include <host_version_info.h>
#include <webview/webview_container.h>
#include <context/copilot_global_context_handle.h>
#include <passive_action/agent/agent_action_handle.h>
#include <wx/aui/auibar.h>
#include <wx/aui/framemanager.h>
#include <copilot_panel_name.h>
#include <copilot_aui_info.h>
#include <kicad_copilot_editors.h>
#include <get_copilot_host_version.h>
#include <widgets/wx_aui_utils.h>
#include <copilot_panel_settings.h>
#include <copilot_clipboard.h>
#include <tool/actions.h>


template <class Derived, class ContextT>

class COPILOT_PANEL_CONTAINER
{
public:
    COPILOT_PANEL_CONTAINER( wxAuiManager& aAuiMgr ) :
            m_containerAuiMgr( aAuiMgr ),
            m_copilotGlobalContextHdl(
                    std::make_shared<std::function<COPILOT_GLOBAL_CONTEXT const&()>>(
                            [&]() -> COPILOT_GLOBAL_CONTEXT const&
                            {
                                UpdateCopilotContextCache();
                                return m_copilotContextCache;
                            } ) ),
            m_copilotAgentActionHdl( std::make_shared<AGENT_ACTION_HANDLE_T>(
                    [&]( AGENT_ACTION const& act )
                    {
                        ExecuteAgentAction( act );
                    } ) ),
            m_copilotSelectionHdl( std::make_shared<SELECTION_HDL_T>(
                    [this]()
                    {
                        return GetJsonifySelection();
                    } ) )
    {
    }


    auto self() { return static_cast<Derived*>( this ); }
    auto self() const { return static_cast<const Derived*>( this ); }

    KICAD_COPILOT_EDITORS editor_type() const { return KICAD_COPILOT_EDITORS::unknown; }

    void CopilotPanelInit()
    {
        m_containerCfg = self()->config();
        m_copilotContextCache.host_version_info = get_copilot_host_version( editor_type() );
        m_copilotPanel = new WEBVIEW_CONTAINER(
                self(), { m_copilotContextCache.host_version_info, m_copilotGlobalContextHdl,
                          m_copilotAgentActionHdl, m_copilotSelectionHdl } );
    }

    void InitCopilotAui()
    {
        if( m_copilotPanel )
            m_containerAuiMgr.AddPane( m_copilotPanel, defaultCopilotPaneInfo( self() ) );
    }

    void RecreateCopilotToolBar( auto* aToolBar, auto& aAction )
    {
        if( m_copilotPanel )
        {
            aToolBar->AddScaledSeparator( self() );
            aToolBar->Add( aAction );
        }
    }

    void CopilotPanelShowChangedLanguage()
    {
        if( m_copilotPanel )
        {
            wxAuiPaneInfo& copilot_panel_info = m_containerAuiMgr.GetPane( m_copilotPanel );
            bool           is_shown = copilot_panel_info.IsShown();
            copilot_panel_info.Caption( _( "Copilot" ) );
            copilot_panel_info.Show( is_shown );
        }
    }

    void ToggleCopilot()
    {
        wxAuiPaneInfo& copilot_pane = m_containerAuiMgr.GetPane( CopilotPanelName() );
        ShowCopilot( !copilot_pane.IsShown() );
    }

    void ShowCopilot( bool show = true )
    {
        auto& copilot_pane = m_containerAuiMgr.GetPane( CopilotPanelName() );

        bool now_shown = copilot_pane.IsShown();

        if( now_shown == show )
            return;

        copilot_pane.Show( show );

        if( copilot_pane.IsShown() )
        {
            if( copilot_pane.IsFloating() )
            {
                copilot_pane.FloatingSize( m_containerCfg->copilot_panel_float_width,
                                           m_containerCfg->copilot_panel_float_height );
                m_containerAuiMgr.Update();
            }
            else if( m_containerCfg->copilot_panel_docked_width > 0 )
            {
                // SetAuiPaneSize also updates m_auimgr
                SetAuiPaneSize( m_containerAuiMgr, copilot_pane,
                                m_containerCfg->copilot_panel_docked_width, -1 );
            }
        }
        else
        {
            if( copilot_pane.IsFloating() )
            {
                m_containerCfg->copilot_panel_float_width = copilot_pane.floating_size.x;
                m_containerCfg->copilot_panel_float_height = copilot_pane.floating_size.y;
            }
            else
            {
                m_containerCfg->copilot_panel_docked_width = m_copilotPanel->GetSize().x;
            }

            m_containerAuiMgr.Update();
        }
    }

    void SaveCopilotCnf()
    {
        if( m_copilotPanel )
        {
            auto& m_auimgr = static_cast<Derived*>( this )->m_auimgr;
            auto& copilotPane = m_auimgr.GetPane( CopilotPanelName() );
            m_containerCfg->copilot_panel_show = copilotPane.IsShown();

            if( copilotPane.IsDocked() )
                m_containerCfg->copilot_panel_docked_width = m_copilotPanel->GetSize().x;
            else
            {
                m_containerCfg->copilot_panel_float_height = copilotPane.floating_size.y;
                m_containerCfg->copilot_panel_float_width = copilotPane.floating_size.x;
            }
        }
    }

    void LoadCopilotCnf()
    {
        if( m_copilotPanel )
        {
            auto& copilotPane = m_containerAuiMgr.GetPane( CopilotPanelName() );
            copilotPane.Show( m_containerCfg->copilot_panel_show );
            if( m_containerCfg->copilot_panel_show )
            {
                SetAuiPaneSize( m_containerAuiMgr, copilotPane,
                                m_containerCfg->copilot_panel_docked_width, -1 );
            }
        }
    }

    void UpdateCopilotContextCache() {}

    // Copilot commands Dispatcher
    void FireCopilotCommand( std::string const& aCmdType ) { WXUNUSED( aCmdType ); }

    void ExecuteAgentAction( AGENT_ACTION const& aAction )
    {
        try
        {
            if( "paste" == aAction.action )
            {
                return CopilotRunPaste( aAction.context );
            }

            wxLogWarning( "Unknown action received: %s", aAction.action.c_str() );
        }
        catch( std::exception const& e )
        {
            wxLogWarning( "Exception caught: %s", e.what() );
        }
    }

    void SetHasSelection( bool aHasSelection )
    {
        self()->CallAfter(
                [=, this]()
                {
                    if( m_copilotPanel )
                        m_copilotPanel->on_host_selection_changed( aHasSelection );
                } );
    }

    std::string GetJsonifySelection() const { return ( nlohmann::json() ).dump(); }


    void CopilotRunPaste( std::string const& aContent )
    {
        WriteClipBoard( aContent );
        self()->GetToolManager()->RunAction( ACTIONS::paste );
    }


protected:
    WEBVIEW_CONTAINER* m_copilotPanel{};
    ContextT           m_copilotContextCache;

private:
    COPILOT_PANEL_SETTINGS*          m_containerCfg{};
    wxAuiManager&                    m_containerAuiMgr;
    COPILOT_GLOBAL_CONTEXT_OWNED_HDL m_copilotGlobalContextHdl;
    AGENT_ACTION_OWNED_HANDLE        m_copilotAgentActionHdl;
    COPILOT_SELECTION_OWNED_HDL      m_copilotSelectionHdl;
};

#endif
