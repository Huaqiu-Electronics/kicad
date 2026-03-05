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
#include "webview_library/webview_library_settings.h"
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
#include <tool/tool_manager.h>
#include <settings/webview_settings_manager.h>
#include <webview_library/webview_library_aui_settings.h>
#include <utils/add_parameter_to_url.h>


class WEBVIEW_LIBRARY_CONTAINER;
template <class Derived>

class WEBVIEW_LIBRARY_PANEL_CONTAINER
{
    wxAuiManager& m_auiMgr;

public:
    WEBVIEW_LIBRARY_PANEL_CONTAINER( wxAuiManager& aAuiMgr ) :
            m_auiMgr( aAuiMgr )
    {
    }

    auto self() { return static_cast<Derived*>( this ); }
    auto self() const { return static_cast<const Derived*>( this ); }

    void WebViewLibraryInit()
    {
        m_webSymbolChooser = new WEBVIEW_LIBRARY_CONTAINER(
                self()->GetHostVersionInfo(), self(),
                add_parameter_to_url( WEBVIEW_SETTINGS_MANAGER::get_instance().get_library_url(),
                                      self()->GetHostVersionInfo() ),
                { {}, self()->GetCopilotAgentActionHandle() } );

        m_webViewLibrarySettings = &self()->config()->m_WebViewLibrarySettings;
    }

    void InitWebViewLibraryAui()
    {
        if( m_webSymbolChooser )
        {
            m_auiMgr.AddPane( m_webSymbolChooser, DefaultWebviewLibraryPanelInfo( self() ) );
        }
    }

    void WebViewLibraryShowChangedLanguage()
    {
        if( m_webSymbolChooser )
        {
            wxAuiPaneInfo& web_panel_info = m_auiMgr.GetPane( WebviewLibraryName() );
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
                web_pane.FloatingSize( m_webViewLibrarySettings->panel_float_width,
                                       m_webViewLibrarySettings->panel_float_height );
                m_auiMgr.Update();
            }
            else if( m_webViewLibrarySettings->panel_docked_width > 0 )
            {
                // SetAuiPaneSize also updates m_auimgr
                SetAuiPaneSize( m_auiMgr, web_pane, m_webViewLibrarySettings->panel_docked_width, -1 );
            }
        }
        else
        {
            if( web_pane.IsFloating() )
            {
                m_webViewLibrarySettings->panel_float_width = web_pane.floating_size.x;
                m_webViewLibrarySettings->panel_float_height = web_pane.floating_size.y;
            }
            else
            {
                m_webViewLibrarySettings->panel_docked_width = m_webSymbolChooser->GetSize().x;
            }

            m_auiMgr.Update();
        }
    }

    void LoadWebViewLibraryCnf()
    {
        if( m_webSymbolChooser )
        {
            auto&      webPane = m_auiMgr.GetPane( WebviewLibraryName() );
            const auto show = m_webViewLibrarySettings->panel_show;
            webPane.Show( show );

            if( show )
            {
                SetAuiPaneSize( m_auiMgr, webPane, m_webViewLibrarySettings->panel_docked_width, -1 );
            }
        }
    }


    void SaveWebViewLibraryCnf()
    {
        if( m_webSymbolChooser )
        {
            auto&      webPane = m_auiMgr.GetPane( WebviewLibraryName() );
            const auto show = webPane.IsShown();
            m_webViewLibrarySettings->panel_show = show;

            if( webPane.IsDocked() )
                m_webViewLibrarySettings->panel_docked_width = m_webSymbolChooser->GetSize().x;
            else
            {
                m_webViewLibrarySettings->panel_float_height = webPane.floating_size.y;
                m_webViewLibrarySettings->panel_float_width = webPane.floating_size.x;
            }
        }
    }

protected:
    WEBVIEW_LIBRARY_CONTAINER* m_webSymbolChooser;
    // Shortcut to the app settings
    WEBVIEW_LIBRARY_SETTINGS* m_webViewLibrarySettings;
};

#endif
