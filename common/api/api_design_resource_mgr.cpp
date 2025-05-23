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

#include <project.h>
#include <api/api_design_resource_mgr.h>
#include <kicad_curl/kicad_curl_easy.h>
#include <optional>
#include <wx/debug.h>
#include <wx/log.h>
#include <eda_base_frame.h>

static auto checkServerResponse( auto& aCurl )
{
    int statusCode = aCurl.GetResponseStatusCode();
    if( statusCode != 200 )
    {
        wxLogWarning( "HTTP error: %d", statusCode );
        return false;
    }

    return true;
}

API_DESIGN_RESOURCE_MGR::API_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) : m_frame( aFrame )
{
}

API_DESIGN_RESOURCE_MGR::~API_DESIGN_RESOURCE_MGR()
{
}

std::optional<std::string> API_DESIGN_RESOURCE_MGR::download_url( std::string const& aUrl )
{
    KICAD_CURL_EASY curl;
    curl.SetURL( aUrl );
    curl.Perform();

    if( !checkServerResponse( curl ) )
    {
        return {};
    }

    wxLogWarning( curl.GetBuffer().c_str() );

    return std::make_optional( curl.GetBuffer() );
}


wxString API_DESIGN_RESOURCE_MGR::getProjectPath() const
{
    return m_frame.Prj().GetProjectPath();
}
