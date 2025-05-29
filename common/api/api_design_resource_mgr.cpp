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

#include "fmt/format.h"
#include <project.h>
#include <api/api_design_resource_mgr.h>
#include <kicad_curl/kicad_curl_easy.h>
#include <optional>
#include <wx/debug.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <eda_base_frame.h>
#include <api/api_design_resource.h>
#include <wildcards_and_files_ext.h>
#include <wx/string.h>
#include <wx/wfstream.h>
#include <wx/log.h>

constexpr auto kProjectResourceDirName = "kicad_api_libs";
constexpr auto kProjectPrettyName = "kicad_api_lib";
constexpr auto kProject3DModelName = "3d_models";
constexpr auto kTraceResourceIO = "API_RESOURCE_IO";


API_DESIGN_RESOURCE_MGR::API_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) : m_frame( aFrame )
{
}

API_DESIGN_RESOURCE_MGR::~API_DESIGN_RESOURCE_MGR()
{
}

std::optional<std::string> API_DESIGN_RESOURCE_MGR::DownloadURL( std::string const& aUrl )
{
    KICAD_CURL_EASY curl;
    curl.SetURL( aUrl );
    curl.Perform();

    static const auto checkServerResponse = []( auto& aCurl )
    {
        int statusCode = aCurl.GetResponseStatusCode();
        if( statusCode != 200 )
        {
            wxLogWarning( "HTTP error: %d", statusCode );
            return false;
        }

        return true;
    };

    if( !checkServerResponse( curl ) )
    {
        return {};
    }

    return std::make_optional( curl.GetBuffer() );
}

std::string API_DESIGN_RESOURCE_MGR::ExtractFileName( std::string const& aUrl )
{
    auto pos = aUrl.find_last_of( '/' );

    if( pos == std::string::npos || pos == aUrl.length() - 1 )
    {
        return {};
    }

    return aUrl.substr( pos + 1 );
}

std::string API_DESIGN_RESOURCE_MGR::ExtactFileExtension( std::string const& aUrl )
{
    auto pos = aUrl.find_last_of( '.' );

    if( pos == std::string::npos || pos == aUrl.length() - 1 )
    {
        return {};
    }

    return aUrl.substr( pos + 1 );
}

std::string API_DESIGN_RESOURCE_MGR::RemoveFileExt( std::string const& aFileName )
{
    auto pos = aFileName.find_last_of( '.' );

    if( pos == std::string::npos || pos == aFileName.length() - 1 )
    {
        return aFileName;
    }

    return aFileName.substr( 0, pos );
}

std::string API_DESIGN_RESOURCE_MGR::FormatSymbolFpPropertyValue( std::string const& aSymbolName )
{
    return fmt::format( "{}:{}", GetAPIFootprintPrettyName(), aSymbolName );
}

std::string API_DESIGN_RESOURCE_MGR::GetAPIFootprintPrettyName()
{
    return kProjectPrettyName;
}


std::string API_DESIGN_RESOURCE_MGR::GetAPIFootprintPrettyDirName()
{
    return fmt::format( "{}.{}", GetAPIFootprintPrettyName(),
                        FILEEXT::KiCadFootprintLibPathExtension );
}

std::string API_DESIGN_RESOURCE_MGR::FormatToValidFileName( std::string const& aComponentName )
{
    static const std::string invalidChars = "\\/:*?\"<>|";
    std::string              result = aComponentName;

    for( char& c : result )
    {
        if( invalidChars.find( c ) != std::string::npos )
            c = '_';
    }

    // Optionally, trim whitespace from both ends
    size_t start = result.find_first_not_of( " \t\r\n" );
    size_t end = result.find_last_not_of( " \t\r\n" );
    if( start == std::string::npos )
        return ""; // All whitespace
    return result.substr( start, end - start + 1 );
}

wxString API_DESIGN_RESOURCE_MGR::FormatAPISymbolURI( std::string const& aSymbolName )
{
    return wxString::Format( "${KIPRJMOD}/%s/%s", kProjectResourceDirName, aSymbolName );
}


wxString API_DESIGN_RESOURCE_MGR::GetProjectResourceDirInternal( ProjectResourceDir aType ) const
{
    const auto prjDir = GetProjectDirectory();
    wxFileName resDir( prjDir );
    resDir.AppendDir( kProjectResourceDirName );

    switch( aType )
    {
    case ProjectResourceDir::ROOT_DIR: break;
    case ProjectResourceDir::PRETTY_DIR: resDir.AppendDir( GetAPIFootprintPrettyDirName() ); break;
    case ProjectResourceDir::MODEL_3D_DIR: resDir.AppendDir( kProject3DModelName ); break;

    default:
        wxASSERT_MSG( false, wxString::Format( "Unknown project resource directory type: %d",
                                               static_cast<int>( aType ) ) );
        return {};
    }

    return resDir.GetFullPath();
}

bool API_DESIGN_RESOURCE_MGR::SetupProjectResourceDirs()
{
    for( int type = ProjectResourceDir::BEGIN; type < ProjectResourceDir::END; ++type )
    {
        const auto target_dir =
                GetProjectResourceDirInternal( static_cast<ProjectResourceDir>( type ) );

        wxFileName resDir( target_dir );

        if( !resDir.DirExists( target_dir ) )
        {
            if( !resDir.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
            {
                wxLogError( wxString::Format( _( "Cannot create project resource path '%s'." ),
                                              resDir.GetPath() ) );
                return false;
            }
        }
    }

    return true;
}

wxString API_DESIGN_RESOURCE_MGR::GetProjectApiFpUri() const
{
    return wxString::Format( "${KIPRJMOD}/%s/%s", kProjectResourceDirName,
                             GetAPIFootprintPrettyDirName() );
}

wxString API_DESIGN_RESOURCE_MGR::GetProjectDirectory() const
{
    return m_frame.Prj().GetProjectPath();
}

std::optional<wxString> API_DESIGN_RESOURCE_MGR::GetProjectResourceDir( ProjectResourceDir aType )
{
    if( !SetupProjectResourceDirs() )
    {
        wxLogError( "Failed to set up project resource directories." );
        return {};
    }

    return GetProjectResourceDirInternal( aType );
}


std::optional<wxString>
API_DESIGN_RESOURCE_MGR::GetProjectResourceSavePath( ProjectResourceDir aType,
                                                     wxString const&    aName )
{
    const auto resDir = GetProjectResourceDir( aType );

    if( !resDir )
    {
        wxLogError( "Failed to get project resource directory." );
        return {};
    }

    wxFileName savePath( *resDir );
    savePath.SetFullName( aName );
    return savePath.GetFullPath();
}

bool API_DESIGN_RESOURCE_MGR::WriteProjectResource( wxString const&    aPath,
                                                    std::string const& aContent )
{
    try
    {
        wxFFileOutputStream fileStream( aPath, "wb" );

        if( !fileStream.IsOk() || !fileStream.WriteAll( aContent.c_str(), aContent.size() ) )
        {
            wxLogTrace( kTraceResourceIO, wxT( "Warning: could not save %s" ), aPath );
        }

        fileStream.Close();
    }
    catch( const std::exception& e )
    {
        wxLogTrace( kTraceResourceIO, wxT( "Catch error: could not save %s. IO error %s" ), aPath,
                    e.what() );
        return false;
    }

    return true;
}

bool API_DESIGN_RESOURCE_MGR::WriteProjectResource( ProjectResourceDir aType, wxString const& aName,
                                                    std::string const& aContent )
{
    const auto savePath = GetProjectResourceSavePath( aType, aName );

    if( !savePath )
    {
        wxLogError( "Failed to get project resource save path." );
        return false;
    }

    return WriteProjectResource( *savePath, aContent );
}
