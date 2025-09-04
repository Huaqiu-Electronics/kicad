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

#ifndef WEBVIEW_LIBRARY_PANEL_CONTAINER_H
#define WEBVIEW_LIBRARY_PANEL_CONTAINER_H


#include "webview_library/webview_library_caption.h"
#include <webview_library/webview_library_container.h>
#include <tool/action_toolbar.h>
#include <fmt/format.h>
#include <passive_action/agent/agent_action.h>
#include <magic_enum.hpp>
#include <host_version_info.h>
#include <wx/aui/auibar.h>
#include <wx/aui/framemanager.h>
#include <kicad_copilot_editors.h>
#include <get_copilot_host_version.h>
#include <widgets/wx_aui_utils.h>
#include <tool/actions.h>
#include <utils/web_utils.h>
#include <tool/tool_manager.h>
#include <settings/webview_settings_manager.h>
#include <webview_library/webview_library_aui_settings.h>
#include <tool/actions.h>


class WEBVIEW_LIBRARY_CONTAINER;
template <class Derived>

class WEBVIEW_LIBRARY_PANEL_CONTAINER
{
    wxAuiManager& m_auiMgr;

public:
    WEBVIEW_LIBRARY_PANEL_CONTAINER( wxAuiManager& aAuiMgr ) : m_auiMgr( aAuiMgr ) {}

    auto self() { return static_cast<Derived*>( this ); }
    auto self() const { return static_cast<const Derived*>( this ); }

    void WebViewLibraryInit()
    {
        m_webSymbolChooser = new WEBVIEW_LIBRARY_CONTAINER(
                self(),
                add_parameter_to_url( WEBVIEW_SETTINGS_MANAGER::get_instance().get_library_url(),
                                      self()->GetHostVersionInfo() ),
                { {}, self()->GetCopilotAgentActionHandle() } );
    }

    void InitWebViewLibraryAui()
    {
        if( m_webSymbolChooser )
        {
            m_auiMgr.AddPane( m_webSymbolChooser, DefaultWebviewLibraryPanelInfo( self() ) );
        }
    }

    void RecreateWebViewLibraryToolBar( bool aAddSeparator = true )
    {
        if( aAddSeparator )
            self()->m_mainToolBar->AddScaledSeparator( self() );

        self()->m_mainToolBar->Add( ACTIONS::toggleWebViewLibrary );
    }

    void WebViewLibraryShowChangedLanguage()
    {
        if( m_webSymbolChooser )
        {
            wxAuiPaneInfo& web_panel_info = m_auiMgr.GetPane( m_webSymbolChooser );
            bool           is_shown = web_panel_info.IsShown();
            web_panel_info.Caption( GetWebviewLibraryCaption() );
            web_panel_info.Show( is_shown );
        }
    }

    void ToggleWebViewLibrary()
    {
        wxAuiPaneInfo& web_pane = m_auiMgr.GetPane( WebviewLibraryName() );
        ShowWebViewLibrary( !web_pane.IsShown() );
    }

    void ShowWebViewLibrary( bool aShow )
    {
        auto& web_pane = m_auiMgr.GetPane( WebviewLibraryName() );

        bool now_shown = web_pane.IsShown();

        if( now_shown == aShow )
            return;

        web_pane.Show( aShow );

        if( web_pane.IsShown() )
        {
            if( web_pane.IsFloating() )
            {
                web_pane.FloatingSize( self()->config()->m_WebViewLibrarySettings.panel_float_width,
                                       self()->config()->m_WebViewLibrarySettings.panel_float_height );
                m_auiMgr.Update();
            }
            else if( self()->config()->m_WebViewLibrarySettings.panel_docked_width > 0 )
            {
                // SetAuiPaneSize also updates m_auimgr
                SetAuiPaneSize( m_auiMgr, web_pane, self()->config()->m_WebViewLibrarySettings.panel_docked_width, -1 );
            }
        }
        else
        {
            if( web_pane.IsFloating() )
            {
                self()->config()->m_WebViewLibrarySettings.panel_float_width = web_pane.floating_size.x;
                self()->config()->m_WebViewLibrarySettings.panel_float_height = web_pane.floating_size.y;
            }
            else
            {
                self()->config()->m_WebViewLibrarySettings.panel_docked_width = m_webSymbolChooser->GetSize().x;
            }

            m_auiMgr.Update();
        }
    }

    void LoadWebViewLibraryCnf()
    {
        if( m_webSymbolChooser )
        {
            auto& webPane = m_auiMgr.GetPane( WebviewLibraryName() );
            webPane.Show( self()->config()->m_WebViewLibrarySettings.panel_show );
            if( self()->config()->m_WebViewLibrarySettings.panel_show )
            {
                SetAuiPaneSize( m_auiMgr, webPane, self()->config()->m_WebViewLibrarySettings.panel_docked_width, -1 );
            }
        }
    }


    void SaveWebViewLibraryCnf()
    {
        if( m_webSymbolChooser )
        {
            auto& webPane = m_auiMgr.GetPane( WebviewLibraryName() );
            self()->config()->m_WebViewLibrarySettings.panel_show = webPane.IsShown();

            if( webPane.IsDocked() )
                self()->config()->m_WebViewLibrarySettings.panel_docked_width = m_webSymbolChooser->GetSize().x;
            else
            {
                self()->config()->m_WebViewLibrarySettings.panel_float_height = webPane.floating_size.y;
                self()->config()->m_WebViewLibrarySettings.panel_float_width = webPane.floating_size.x;
            }
        }
    }

protected:
    WEBVIEW_LIBRARY_CONTAINER* m_webSymbolChooser;
};

#endif
