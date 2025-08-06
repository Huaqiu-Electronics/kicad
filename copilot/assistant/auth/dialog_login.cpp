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

#include <auth/access_token.h>
#include <auth/dialog_login.h>
#include <wx/event.h>
#include <settings/webview_settings_manager.h>
#include <wx/log.h>
#include <passive_action/passive_action_container.h>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>
#include <magic_enum.hpp>
#include <utils/extract_host_from_url.h>
#include <utils/web_utils.h>
#include <utils/wstring_to_string.h>


enum FIXED_SIZE
{
    WIDTH = 400,
    HEIGHT = 600
};

DIALOG_LOGIN::DIALOG_LOGIN( wxWindow* aParent ) :
        DIALOG_WEBVIEW( aParent, wxID_ANY, _( "Login" ), "dialog_login", { WIDTH, HEIGHT } )
{
    SetWindowStyle( wxDEFAULT_DIALOG_STYLE | wxBORDER_NONE );
    LoadUrl( WEBVIEW_SETTINGS_MANAGER::get_instance().get_login_path() );
}

DIALOG_LOGIN::~DIALOG_LOGIN()
{
}

bool DIALOG_LOGIN::Success() const
{
    return WEBVIEW_SETTINGS_MANAGER::get_instance().is_token_valid();
}