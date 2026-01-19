/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2026 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2026 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef VALIDATE_HUAQIU_TOKEN_H
#define VALIDATE_HUAQIU_TOKEN_H

#include "httplib_wrapper.h"
#include <settings/webview_settings_manager.h>
#include <webview/webview_controller.h>
#include <nlohmann/json.hpp>


namespace
{
struct PROFILE_INFO
{
    bool suc;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PROFILE_INFO, suc )
};

} // namespace


inline auto validate_huaqiu_token() -> bool
{
    if( !WEBVIEW_SETTINGS_MANAGER::get_instance().has_local_token() )
        return false;


    static httplib::SSLClient cli( "customer.huaqiu.com" );

    auto res = cli.Get( "/web/index/user/profile/info",
                        { { "Authorization", WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token } } );

    if( res && res->status == 200 )
    {
        try
        {
            if( nlohmann::json::parse( res->body ).get<PROFILE_INFO>().suc )
                return true;
        }

        catch( ... )
        {
        }
    }

    WEBVIEW_CONTROLLER::Get().OnLogout();
    return false;
}

#endif
