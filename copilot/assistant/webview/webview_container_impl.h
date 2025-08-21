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

#ifndef WEBVIEW_CONTAINER_IMPL_H
#define WEBVIEW_CONTAINER_IMPL_H


#include <host_copilot_handles.h>
#include <utils/httplib_wrapper.h>
#include <context/copilot_global_context_handle.h>
#include <context/copilot_global_context.h>


class wxWebView;
class DIALOG_WEBVIEW;
class DIALOG_IMAGE_VIEWER;

struct WEBVIEW_CONTAINER_IMPL
{
    std::set<std::string> _consumed_global_ctx_keys{};
    DIALOG_IMAGE_VIEWER*  _image_viewer{};
    DIALOG_WEBVIEW*       _ecad_viewer{};


    wxWebView*                 _browser;
    HOST_COPILOT_HANDLES       _host_copilot_handles;
    std::optional<std::string> _primary_url;
};


#endif
