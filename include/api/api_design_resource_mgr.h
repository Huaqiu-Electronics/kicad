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

#ifndef API_DESIGN_RESOURCE_MGR_H
#define API_DESIGN_RESOURCE_MGR_H

#include <string>
#include <optional>
#include <wx/string.h>

class EDA_BASE_FRAME;
struct DesignBlockConent;
struct ComponentContent;

class API_DESIGN_RESOURCE_MGR
{
public:
    enum ProjectResourceDir
    {
        BEGIN = 0,

        ROOT_DIR = BEGIN,
        SYMBOL_DIR = ROOT_DIR,
        PRETTY_DIR,
        MODEL_3D_DIR,

        END
    };


    enum API_LIB_TABLE_T
    {
        API_LIB_SYMBOL_TABLE,
        API_LIB_FP_TABLE,
        API_LIB_DESIGN_BLOCK_TABLE
    };


    API_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame );

    virtual ~API_DESIGN_RESOURCE_MGR();

    // Download the url conent
    static std::optional<std::string> DownloadURL( std::string const& aUrl );

    static std::string ExtractFileName( std::string const& aUrl );

    static std::string ExtactFileExtension( std::string const& aUrl );

    static std::string RemoveFileExt( std::string const& aFileName );

    static std::string GetAPIFootprintPrettyDirName();

    wxString GetProjectApiFpUri() const;

    wxString GetProjectDirectory() const;

    //  Get the project resource directory for the given type.
    std::optional<wxString> GetProjectResourceDir( ProjectResourceDir aType );
    
    // Get the project resource save path for the given type and name.
    std::optional<wxString> GetProjectResourceSavePath( ProjectResourceDir aType, wxString const& aName );

    bool WriteProjectResource( ProjectResourceDir aType, wxString const& aName, std::string const& aContent );

    bool WriteProjectResource( wxString const& aPath, std::string const& aContent );

private:
    wxString GetProjectResourceDirInternal( ProjectResourceDir aType ) const;

    bool SetupProjectResourceDirs();

protected:
    EDA_BASE_FRAME& m_frame;
};





#endif
