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

#include <settings/webview_settings_manager.h>
#include <settings/immutable_settings.h>
#include <settings/mutable_settings.h>
#include <chrono>

#include "config_io_helper.h"


constexpr auto kMutableSettingsFileName = "mutable_settings.json";
constexpr auto kImmutableSettingsFileName = "immutable_settings.json";

WEBVIEW_SETTINGS_MANAGER::WEBVIEW_SETTINGS_MANAGER() :
        _immutable_settings( new IMMUTABLE_SETTINGS ), _mutable_settings( new MUTABLE_SETTINGS )
{
    load_cnf( get_copilot_setting_path( kImmutableSettingsFileName ), *_immutable_settings );
    load_cnf( get_copilot_setting_path( kMutableSettingsFileName ), *_mutable_settings );

    auto url = _immutable_settings->copilot_webview_settings.url;

    if( url.back() == '/' )
        url = url.substr( 0, url.length() - 1 );


    _webview_chat_path = fmt::format( "{}/#{}", _immutable_settings->copilot_webview_settings.url,
                                      _immutable_settings->copilot_webview_settings.path.chat );

    _webview_image_viewer_path = fmt::format( "{}/#{}", _immutable_settings->copilot_webview_settings.url,
                                              _immutable_settings->copilot_webview_settings.path.image_viewer );

    _login_path = fmt::format( "{}/#{}", _immutable_settings->copilot_webview_settings.url,
                               _immutable_settings->copilot_webview_settings.path.login );
}

WEBVIEW_SETTINGS_MANAGER::~WEBVIEW_SETTINGS_MANAGER()
{
    save_cnf( get_copilot_setting_path( kMutableSettingsFileName ), *_mutable_settings );
}

WEBVIEW_SETTINGS_MANAGER& WEBVIEW_SETTINGS_MANAGER::get_instance()
{
    static WEBVIEW_SETTINGS_MANAGER instance;
    return instance;
}

std::string WEBVIEW_SETTINGS_MANAGER::get_copilot_setting_dir()
{
    static const auto kCopilotSettingDir = ([]{
        wxFileName path;
        const auto mk_dir =[&]{        
            if( !path.DirExists() )
            {
                if( !wxMkdir( path.GetPath() ) )
                {
                    wxLogTrace( "WEBVIEW_SETTINGS_MANAGER",
                                wxT( "get_copilot_setting_dir(): Path %s missing and could not be created!" ),
                                path.GetPath() );
                }
            }
        };

        path.AssignDir(  wxStandardPaths::Get().GetUserConfigDir() );
        
        for(const auto& dir : { TO_STR( KICAD_CONFIG_DIR ) , TO_STR( COPILOT_CONFIG_DIR ) } ){
            path.AppendDir( dir );
            mk_dir();
        }
        
        return path.GetPath().ToStdString( wxConvUTF8 );
    })();


    return kCopilotSettingDir;
}

std::string WEBVIEW_SETTINGS_MANAGER::get_copilot_setting_path( std::string const& file_name )
{
    return get_copilot_setting_dir() + "/" + file_name;
}


std::string const& WEBVIEW_SETTINGS_MANAGER::get_data_buried_point_host() const
{
    return _immutable_settings->data_buried_point_settings.host;
}

std::string const& WEBVIEW_SETTINGS_MANAGER::get_data_buried_point_endpoint() const
{
    return _immutable_settings->data_buried_point_settings.endpoint;
}

std::string const& WEBVIEW_SETTINGS_MANAGER::get_copilot_webview_url() const
{
    return _immutable_settings->copilot_webview_settings.url;
}

COPILOT_WEBVIEW_PATH const& WEBVIEW_SETTINGS_MANAGER::get_copilot_webview_path() const
{
    return _immutable_settings->copilot_webview_settings.path;
}

std::string const& WEBVIEW_SETTINGS_MANAGER::get_webview_chat_path() const
{
    return _webview_chat_path;
}

std::string const& WEBVIEW_SETTINGS_MANAGER::get_webview_image_viewer_path() const
{
    return _webview_image_viewer_path;
}

std::string const& WEBVIEW_SETTINGS_MANAGER::get_login_path() const
{
    return _login_path;
}

std::optional<DIALOG_GEOMETRY> WEBVIEW_SETTINGS_MANAGER::get_dialog_geometry( std::string const& aKey ) const
{
    if( !_mutable_settings->dialog_geometries.count( aKey ) )
    {
        return {};
    }

    return _mutable_settings->dialog_geometries.at( aKey );
}

void WEBVIEW_SETTINGS_MANAGER::set_dialog_viewer_geometry( std::string const& aKey, DIALOG_GEOMETRY const& geometry )
{
    _mutable_settings->dialog_geometries[aKey] = geometry;
}


std::optional<ACCESS_TOKEN> WEBVIEW_SETTINGS_MANAGER::get_access_token() const
{
    return _mutable_settings->access_token;
}

void WEBVIEW_SETTINGS_MANAGER::set_access_token( ACCESS_TOKEN const& token )
{
    _mutable_settings->access_token = token;
}

bool WEBVIEW_SETTINGS_MANAGER::is_token_valid() const
{
    return _mutable_settings->access_token.has_value()
           && _mutable_settings->access_token->expires_at
                      > std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
}