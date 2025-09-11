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

#ifndef WEBVIEW_INTERNAL_CMD_TYPE_H
#define WEBVIEW_INTERNAL_CMD_TYPE_H


enum class WEB_HOST_INTERNAL_CMD_TYPE
{
    fetch_global_context_from_host,
    show_image,
    show_ecad_viewer,

    update_access_token,
    launch_login_dialog,
    logout,

    open_url_in_webview_dialog,

    // Adding design block is handled in the agent API in the schematic
    // add_design_block,
    finish_adding_design_block,

    add_component,
    finish_adding_component,

    add_component_type,
    finish_adding_component_type,

    add_design_block_type,
    finish_adding_design_block_type,

    // Launch a webview dialog with URL
    show_modal,
    end_modal,

};


#endif
