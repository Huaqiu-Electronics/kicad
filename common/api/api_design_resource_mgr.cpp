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

    wxLogWarning( curl.GetBuffer().c_str() );

    return std::make_optional( curl.GetBuffer() );
}

std::string API_DESIGN_RESOURCE_MGR::ExtractName( std::string const& aUrl )
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


wxString API_DESIGN_RESOURCE_MGR::GetProjectResourceDirInternal( ProjectResourceDir aType ) const
{
    wxFileName resDir( GetProjectDirectory() );
    resDir.AppendDir( kProjectResourceDirName );

    switch( aType )
    {
    case ProjectResourceDir::ROOT_DIR: break;
    case ProjectResourceDir::PRETTY_DIR:
        resDir.AppendDir( wxString::Format( "%s.%s", kProjectPrettyName, FILEEXT::KiCadFootprintLibPathExtension ) );
        break;
    case ProjectResourceDir::MODEL_3D_DIR: resDir.AppendDir( kProject3DModelName ); break;

    default:
        wxASSERT_MSG( false,
                      wxString::Format( "Unknown project resource directory type: %d", static_cast<int>( aType ) ) );
        return {};
    }

    return resDir.GetFullPath();
}

bool API_DESIGN_RESOURCE_MGR::SetupProjectResourceDirs()
{
    for( int type = ProjectResourceDir::BEGIN; type < ProjectResourceDir::END; ++type )
    {
        wxFileName resDir( GetProjectResourceDirInternal( static_cast<ProjectResourceDir>( type ) ) );

        if( !resDir.DirExists() )
        {
            if( !resDir.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
            {
                wxLogError( wxString::Format( _( "Cannot create project resource path '%s'." ), resDir.GetPath() ) );
                return false;
            }
        }
    }

    return true;
}


wxString API_DESIGN_RESOURCE_MGR::GetProjectDirectory() const
{
    return m_frame.Prj().GetProjectDirectory();
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


std::optional<wxString> API_DESIGN_RESOURCE_MGR::GetProjectResourceSavePath( ProjectResourceDir aType,
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

bool API_DESIGN_RESOURCE_MGR::WriteProjectResource( wxString const& aPath, std::string const& aContent )
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
        wxLogTrace( kTraceResourceIO, wxT( "Catch error: could not save %s. IO error %s" ), aPath, e.what() );
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

    return savePath && WriteProjectResource( *savePath, aContent );
}

bool API_DESIGN_RESOURCE_MGR::SaveComponentToProject( ComponentContent const& aComponent )
{
    if( !WriteProjectResource( SYMBOL_DIR, aComponent.symbol.name, aComponent.symbol.content ) )
    {
        wxLogError( "Failed to save symbol %s to project", aComponent.symbol.name );
        return false;
    }

    SYMBOL_LIB_TABLE* sym_table;


    for( const auto& footprint : aComponent.footprints )
    {
        if( !WriteProjectResource( PRETTY_DIR, footprint.footprint.name, footprint.footprint.content ) )
        {
            wxLogError( "Failed to save footprint %s to project", footprint.footprint.name );
            continue;
        }

        for( const auto& model : footprint.models )
        {
            if( !WriteProjectResource( MODEL_3D_DIR, model.name, model.content ) )
            {
                wxLogError( "Failed to save 3D model %s to project", model.name );
                continue;
            }
        }
    }

    return true;
}

bool API_DESIGN_RESOURCE_MGR::SaveDesignBlockToProject( DesignBlockConent const& aComponent )
{
    return {};
}


bool API_DESIGN_RESOURCE_MGR::UpdateLibTable( API_LIB_TABLE_T aLibT, const wxString& aNickName, const wxString& aURI )
{
    WXUNUSED( aLibT );
    WXUNUSED( aNickName );
    WXUNUSED( aURI );
    return false;
}
