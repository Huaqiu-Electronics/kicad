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
#include <settings/site_env.h>
#include <fmt/format.h>

inline const auto& get_edn_cn_url()
{
    static const auto kEdnCnUrl =
            fmt::format( "{}://{}", WEBVIEW_SETTINGS_MANAGER::get_instance().use_https() ? "https" : "http",
                         WEBVIEW_SETTINGS_MANAGER::get_instance().get_edn_cn_host() );
    return kEdnCnUrl;
}

inline auto get_eda_cn_url_from_path( const std::string& aPath )
{
    return fmt::format( "{}{}", get_edn_cn_url(), aPath );
}


std::string WEBVIEW_SETTINGS_MANAGER::get_edn_cn_host() const
{
    return _site_env->eda_cn_host;
}

std::string WEBVIEW_SETTINGS_MANAGER::get_edn_cn_url() const
{
    return get_edn_cn_url();
}

std::string WEBVIEW_SETTINGS_MANAGER::get_add_design_block_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.add_design_block );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_add_design_block_type_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.add_design_block_type );
}


std::string WEBVIEW_SETTINGS_MANAGER::get_add_component_type_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.add_component_type );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_add_component_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.add_component );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_login_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.login );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_pcb_quote_path() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.pcb_quote );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_smt_quote_path() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.smt_quote );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_news_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.news_url );
}

std::string WEBVIEW_SETTINGS_MANAGER::get_library_url() const
{
    return get_eda_cn_url_from_path( _site_env->site_paths.library );
}


bool WEBVIEW_SETTINGS_MANAGER::use_https() const
{
    return _site_env->use_https;
}

std::string get_news_url()
{
    return WEBVIEW_SETTINGS_MANAGER::get_instance().get_news_url();
}
