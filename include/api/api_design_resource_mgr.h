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

    /**
     * @brief Download the content of a design resource from the given URL.
     * @param aUrl The url to a design resource e.g. symbol ,footprint 
     * @return The string buffer of the url
     */
    static std::optional<std::string> DownloadURL( std::string const& aUrl );

    static std::string ExtractName( std::string const& aUrl );

    static std::string ExtactFileExtension( std::string const& aUrl );

    wxString GetProjectDirectory() const;

    /**
     * @brief Get the project resource directory for the given type.
     * @param aType The type of resource directory to get.
     * @return The path to the project resource directory, or std::nullopt if not set.
     */
    std::optional<wxString> GetProjectResourceDir( ProjectResourceDir aType );

    /**
     * @brief Get the project resource save path for the given type and name.
     * @param aType The type of resource directory to get.
     * @param aName The name of the resource.
     * @return The path to the project resource save path, or std::nullopt if not set.
     */
    std::optional<wxString> GetProjectResourceSavePath( ProjectResourceDir aType, wxString const& aName );

    bool WriteProjectResource( ProjectResourceDir aType, wxString const& aName, std::string const& aContent );

    bool WriteProjectResource( wxString const& aPath, std::string const& aContent );

    bool SaveComponentToProject( ComponentContent const& aComponent );

    bool SaveDesignBlockToProject( DesignBlockConent const& aComponent );

    // TODO : Currently it's done in the sub class ,how about exposing the corresponding LIB_TABLE getter through KiWay to get a centralized API ?
    virtual bool UpdateLibTable( API_LIB_TABLE_T aLibT, const wxString& aNickName, const wxString& aURI );

private:
    wxString GetProjectResourceDirInternal( ProjectResourceDir aType ) const;

    bool SetupProjectResourceDirs();

protected:
    EDA_BASE_FRAME& m_frame;
};


#endif
