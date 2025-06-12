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

#ifndef API_OPEN_PROJECT_URL_IMPL_H
#define API_OPEN_PROJECT_URL_IMPL_H

#include <api/api_open_project_url.h>
#include <api/api_design_resource_mgr.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>


void OpenProjectZipAchieveUrl( std::string const& aPrjZipUrl )
{
    wxFileName f( wxStandardPaths::Get().GetExecutablePath() );
    wxString   appPath( f.GetPath() );

    const std::optional<std::string> zip_buffer =
            API_DESIGN_RESOURCE_MGR::DownloadURL( aPrjZipUrl );

    if( !zip_buffer )
        return;
}


#endif
