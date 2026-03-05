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

#include <passive_action/agent/agent_action_type.h>
#include <functional>
#include <tool/action_toolbar.h>
#include <fmt/format.h>
#include <memory>
#include <passive_action/agent/agent_action.h>
#include <magic_enum.hpp>
#include <host_version_info.h>
#include <webview/copilot_webview_container.h>
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
#include <settings/get_webview_chat_path.h>
#include <api/api_open_project_url.h>
#include <tool/tool_manager.h>
#include <utils/random_substring.h>
#include <utils/add_parameter_to_url.h>
#include <context/copilot_project_holder.h>
#include <context/copilot_cpp_sdk_handle.h>
#include <context/copilot_editor.h>

template <class Derived, class ContextT, auto EditorTypeV>

class COPILOT_PANEL_CONTAINER : public COPILOT_PROJECT_HOLDER, public COPILOT_EDITOR
{
public:
    COPILOT_PANEL_CONTAINER( wxAuiManager& aAuiMgr ) :
            m_containerAuiMgr( aAuiMgr ),
            m_copilotGlobalContextHdl( std::make_shared<std::function<COPILOT_GLOBAL_CONTEXT const&()>>(
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
                    } ) ),
            m_copilotCppSdkAgentHdl( std::make_shared<CPP_SDK_AGENT_ACTION_HANDLE_T>(
                    [&]( AGENT_ACTION const& act, std::string& response )
                    {
                        ExecuteSdkQueryAgentAction( act, response );
                    } ) )
    {
    }


    auto self() { return static_cast<Derived*>( this ); }
    auto self() const { return static_cast<const Derived*>( this ); }

    KICAD_COPILOT_EDITORS editor_type() const override { return EditorTypeV; }

    // Used by the webview container to index the chat history
    std::string GetSessionId() const
    {
        return fmt::format( "{}:{}", self()->Prj().GetProjectPath().ToStdString(),
                            magic_enum::enum_name( editor_type() ) );
    }

    // Used by the server to identify the chat
    std::string GetChatId() const { return m_chat_id; }

    void CopilotPanelInit()
    {
        m_containerCfg = self()->config();
        m_hostVersionInfo = get_copilot_host_version( editor_type() );
        m_hostVersionInfo.chat_id = m_chat_id;
        m_hostVersionInfo.session_id = GetSessionId();
        m_copilotPanel = new COPILOT_WEBVIEW_CONTAINER(
                m_hostVersionInfo, self(), add_parameter_to_url( get_webview_chat_path(), m_hostVersionInfo ),
                { m_copilotGlobalContextHdl,
                  m_copilotAgentActionHdl,
                  m_copilotSelectionHdl,
                  {},
                  m_copilotCppSdkAgentHdl } ); //m_copilotDsnHdl
        m_copilotContextCache.host_version_info = m_hostVersionInfo;
    }

    void InitCopilotAui()
    {
        if( m_copilotPanel )
            m_containerAuiMgr.AddPane( m_copilotPanel, defaultCopilotPaneInfo( self() ) );
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
                SetAuiPaneSize( m_containerAuiMgr, copilot_pane, m_containerCfg->copilot_panel_docked_width, -1 );
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
                SetAuiPaneSize( m_containerAuiMgr, copilotPane, m_containerCfg->copilot_panel_docked_width, -1 );
            }
        }
    }

    void UpdateCopilotContextCache() {}

    // Copilot commands Dispatcher
    void FireCopilotCommand( std::string const& aCmdType ) { WXUNUSED( aCmdType ); }

    void ExecuteAgentAction( AGENT_ACTION const& aAction ) { return BaseExecuteAgentAction( aAction ); }

    void BaseExecuteAgentAction( AGENT_ACTION const& aAction )
    {
        const auto& context = aAction.context;

        try
        {
            auto t = magic_enum::enum_cast<AGENT_ACTION_TYPE>( aAction.action );

            if( !t.has_value() )
            {
                wxLogWarning( "Unknown action received: %s", aAction.action.c_str() );
                return;
            }

            switch( *t )
            {
            case AGENT_ACTION_TYPE::paste:
            {
                RunPasteAction( context );
                break;
            }
            case AGENT_ACTION_TYPE::open_project_zip_achieve:
            {
                OpenProjectLaunchKiCad( context );
                break;
            }
            case AGENT_ACTION_TYPE::open_project_zip_achieve_in_current:
            {
                OpenProjectInCurrentProcess( context, self() );
                break;
            }
            case AGENT_ACTION_TYPE::toggle_online_library:
            {
                self()->GetToolManager()->RunAction( ACTIONS::toggleWebViewLibrary );
                break;
            }
            default:
            {
                wxLogWarning( "Unsupported action received: %s", aAction.action.c_str() );
                break;
            }
            }
        }
        catch( std::exception const& e )
        {
            wxLogWarning( "Exception caught: %s", e.what() );
        }
    }

    void SetHasSelection( bool aHasSelection )
    {
        if( m_copilotPanel )
            m_copilotPanel->OnHostSelectionChanged( aHasSelection );
    }

    std::string GetJsonifySelection() const { return ( nlohmann::json() ).dump(); }


    void RunPasteAction( std::string const& aContent )
    {
        WriteClipBoard( aContent );
        self()->GetToolManager()->RunAction( ACTIONS::paste );
    }


    auto GetCopilotAgentActionHandle() const { return m_copilotAgentActionHdl; }

    auto GetHostVersionInfo() const { return m_hostVersionInfo; }


    wxString GetProjectPath() const override { return self()->Prj().GetProjectPath(); }

    wxString GetProjectDirectory() const override { return self()->Prj().GetProjectPath(); }

    wxString GetProjectName() const override { return self()->Prj().GetProjectPath(); }

    //freerouting
    void GetDSNFileBase64Data( std::string& filename, std::string& base64Data ) { return; }

    void ImportSpectraBase64Data( const std::string& base64Data ) { return; }

    //CPP SDK
    void ExecuteSdkQueryAgentAction( AGENT_ACTION const& aAction, std::string& agentResult ) { return; }

protected:
    ContextT                   m_copilotContextCache;
    COPILOT_WEBVIEW_CONTAINER* m_copilotPanel{};
    COPILOT_PANEL_SETTINGS*    m_containerCfg{};
    wxAuiManager&              m_containerAuiMgr;

    COPILOT_GLOBAL_CONTEXT_OWNED_HDL m_copilotGlobalContextHdl;
    AGENT_ACTION_OWNED_HANDLE        m_copilotAgentActionHdl;
    COPILOT_SELECTION_OWNED_HDL      m_copilotSelectionHdl;
    std::string                      m_chat_id = random_substring();
    HOST_VERSION_INFO                m_hostVersionInfo;

    CPP_SDK_AGENT_ACTION_OWNED_HANDLE m_copilotCppSdkAgentHdl;
};

#endif
