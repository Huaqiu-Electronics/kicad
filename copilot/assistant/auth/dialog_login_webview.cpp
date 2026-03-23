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

#include <auth/dialog_login_webview.h>

#include <nlohmann/json_fwd.hpp>
#include <settings/webview_settings_manager.h>
#include <webview/webview_controller.h>
#include <utils/httplib_wrapper.h>


class WEBVIEW_LOGIN_SETTINGS_MGR
{
    struct SIZE
    {
        int width = 320;
        int height = 424;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE( SIZE, width, height )
    };
    struct WEBVIEW_LOGIN_SETTINGS
    {
        SIZE        size;
        std::string url = "https://www.eda.cn/login";

        NLOHMANN_DEFINE_TYPE_INTRUSIVE( WEBVIEW_LOGIN_SETTINGS, size, url )
    };


    struct KICAD_WEBVIEW_LOGIN_SETTINGS
    {
        WEBVIEW_LOGIN_SETTINGS kicad_10_0_0;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE( KICAD_WEBVIEW_LOGIN_SETTINGS, kicad_10_0_0 )
    };
    WEBVIEW_LOGIN_SETTINGS _settings;

public:
    static WEBVIEW_LOGIN_SETTINGS_MGR& get_instance()
    {
        static WEBVIEW_LOGIN_SETTINGS_MGR instance;
        return instance;
    }


    WEBVIEW_LOGIN_SETTINGS_MGR()
    {
        try
        {
            httplib::SSLClient           client( "www.eda.cn", 443 );
            KICAD_WEBVIEW_LOGIN_SETTINGS settings;
            auto                         result = client.Get( "/all/webview_login_settings.json" );

            if( !result || result->status != 200 )
            {
                return;
            }

            nlohmann::json::parse( result->body ).get_to( settings );
            _settings = settings.kicad_10_0_0;
        }
        catch( ... )
        {
        }
    }

    wxSize   GetSize() const { return wxSize{ _settings.size.width, _settings.size.height }; }
    wxString GetUrl() const { return _settings.url; }
};


DIALOG_LOGIN_WEBVIEW::DIALOG_LOGIN_WEBVIEW( HOST_VERSION_INFO const& host_version_info, wxWindow* aParent ) :
        DIALOG_WEBVIEW( host_version_info, aParent,
                        _( "Electronic Enthusiasts/Huaqiu Circuit/Huaqiu Mall accounts can log in directly" ),
                        "DIALOG_LOGIN_WEBVIEW",
                        aParent ? aParent->FromDIP( WEBVIEW_LOGIN_SETTINGS_MGR::get_instance().GetSize() )
                                : WEBVIEW_LOGIN_SETTINGS_MGR::get_instance().GetSize() )
{
    WEBVIEW_CONTROLLER::Get().RemoveWebviewContainer( GetWebviewContainer() );
    SetWindowStyle( wxDEFAULT_DIALOG_STYLE | wxBORDER_NONE );
    LoadUrl( WEBVIEW_LOGIN_SETTINGS_MGR::get_instance().GetUrl() );
}

DIALOG_LOGIN_WEBVIEW::~DIALOG_LOGIN_WEBVIEW()
{
}
