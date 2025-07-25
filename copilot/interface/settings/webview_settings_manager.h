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

#ifndef COPILOT_SETTINGS_MANAGER_H
#define COPILOT_SETTINGS_MANAGER_H

#include <memory>
#include <optional>
#include <string>
#include <copilot_global.h>
#include <auth/access_token.h>

struct IMMUTABLE_SETTINGS;
struct MUTABLE_SETTINGS;
struct COPILOT_WEBVIEW_PATH;
struct DIALOG_GEOMETRY;

class COPILOT_API WEBVIEW_SETTINGS_MANAGER
{
public:
    ~WEBVIEW_SETTINGS_MANAGER();

    static WEBVIEW_SETTINGS_MANAGER& get_instance();

    static std::string get_copilot_setting_dir();

    static std::string get_copilot_setting_path( std::string const& file_name );

    std::string const& get_data_buried_point_host() const;

    std::string const& get_data_buried_point_endpoint() const;

    std::string const& get_copilot_webview_url() const;

    COPILOT_WEBVIEW_PATH const& get_copilot_webview_path() const;

    std::string const& get_webview_chat_path() const;

    std::string const& get_webview_image_viewer_path() const;

    std::string const& get_login_path() const;

    std::optional<DIALOG_GEOMETRY> get_dialog_geometry( std::string const& aKey ) const;
    void set_dialog_viewer_geometry( std::string const& aKey, DIALOG_GEOMETRY const& aGeometry );

    std::optional<ACCESS_TOKEN> get_access_token() const;
    void                        set_access_token( ACCESS_TOKEN const& token );
    bool                        is_token_valid() const;

private:
    WEBVIEW_SETTINGS_MANAGER();
    std::unique_ptr<IMMUTABLE_SETTINGS> _immutable_settings;
    std::unique_ptr<MUTABLE_SETTINGS>   _mutable_settings;
    std::string                         _webview_chat_path;
    std::string                         _webview_image_viewer_path;
    std::string                         _login_path;
};


#endif
