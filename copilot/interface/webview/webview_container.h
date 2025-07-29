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

#ifndef WEBVIEW_CONTAINER_H
#define WEBVIEW_CONTAINER_H

#include <host_copilot_handles.h>
#include <string>
#include <wx/panel.h>
#include <wx/log.h>
#include <wx/webview.h>
#include <copilot_global.h>
#include <data_buried_point.h>
#include <memory>
#include <optional>

class DIALOG_IMAGE_VIEWER;
class DIALOG_WEBVIEW;

enum class WEBVIEW_MSG_CHANNELS
{
    POST_MSG,
    HTTP,
};


namespace httplib
{
class Server;
}


struct WEBVIEW_CONTAINER_IMPL;

class COPILOT_API WEBVIEW_CONTAINER : public wxPanel
{
public:
    WEBVIEW_CONTAINER( wxWindow* parent, std::string url, HOST_COPILOT_HANDLES host_copilot_handles,
                       std::optional<std::string>       data_buried_point = std::nullopt,
                       std::shared_ptr<httplib::Server> msgServer = nullptr );

    ~WEBVIEW_CONTAINER() override;

    WEBVIEW_MSG_CHANNELS MsgChannelType() const;

    virtual void ExecuteCommand( const char* cmd );

    virtual void OnHostSelectionChanged( bool has_selection );

protected:
    virtual void OnDocumentLoaded( wxWebViewEvent& evt );

    virtual void OnScriptMessage( wxWebViewEvent& evt );

    virtual void OnScriptResult( wxWebViewEvent& evt );

    virtual void OnNewWindow( wxWebViewEvent& evt );

    virtual void OnTitleChanged( wxWebViewEvent& evt );

    virtual void OnNavigationRequest( wxWebViewEvent& evt );

    virtual void OnNavigationComplete( wxWebViewEvent& evt );

    virtual void OnError( wxWebViewEvent& evt );

    virtual void OnFullScreenChanged( wxWebViewEvent& evt );

    virtual void OnWebChannelMsg( std::string const& msg );

    virtual void OnWebChannelMsgWrapper( std::string const& msg );


private:
    std::unique_ptr<WEBVIEW_CONTAINER_IMPL> _impl;
};

#endif
