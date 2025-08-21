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

#include <webview_library/dialog_add_design_block.h>
#include <settings/webview_settings_manager.h>
#include "panel_add_design_block.h"

DIALOG_ADD_DESIGN_BLOCK::DIALOG_ADD_DESIGN_BLOCK( wxWindow* aParent, std::string design_block ) :
        COPILOT_DIALOG( aParent, _( "Add Design Block" ), "design_block_editor" )
{
    Init( new PANEL_ADD_DESIGN_BLOCK( this, WEBVIEW_SETTINGS_MANAGER::get_instance().get_add_design_block_url(),
                                      std::move( design_block ) ) );
}

DIALOG_ADD_DESIGN_BLOCK::~DIALOG_ADD_DESIGN_BLOCK()
{
}
