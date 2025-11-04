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
#include <memory>
#include <wx/filename.h>
#include <wx/stream.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>


#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/zipstrm.h>
#include <wx/log.h>
#include <wx/dir.h>
#include <wx/process.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/dir.h>
#include <wx/process.h>
#include <optional>


void OpenProjectZipAchieveUrl( const std::string& aPrjZipUrl )
{
    wxFileName exeFile( wxStandardPaths::Get().GetExecutablePath() );
    wxString   appPath = exeFile.GetPath();

    const std::optional<std::string> zip_buffer =
            API_DESIGN_RESOURCE_MGR::DownloadURL( aPrjZipUrl );

    if( !zip_buffer )
        return;

    // Step 1: Write zip to tmp file
    wxString tempDir = wxStandardPaths::Get().GetTempDir();
    wxString zipPath = tempDir + wxFILE_SEP_PATH + "project.zip";

    {
        wxFileOutputStream out( zipPath );
        out.Write( zip_buffer->data(), zip_buffer->size() );
    }

    // Step 2: Create unzip folder
    wxString unzipDir = tempDir + wxFILE_SEP_PATH + "unzipped_project";
    wxMkdir( unzipDir, wxS_DIR_DEFAULT );

    // Step 3: Extract zip
    {
        wxFFileInputStream          in( zipPath );
        wxZipInputStream            zipStream( in );
        std::unique_ptr<wxZipEntry> entry;

        while( ( entry.reset( zipStream.GetNextEntry() ), entry ) )
        {
            wxFileName entryPath( unzipDir, entry->GetName() );

            if( entry->IsDir() )
            {
                wxMkdir( entryPath.GetFullPath(), wxS_DIR_DEFAULT );
            }
            else
            {
                wxMkdir( entryPath.GetPath(), wxS_DIR_DEFAULT );
                wxFileOutputStream out( entryPath.GetFullPath() );
                out.Write( zipStream );
            }
        }
    }

    // Step 4: Search .pro or .kicad_pro file recursively
    wxString foundProPath;

    std::function<bool( const wxString& )> FindProFile = [&]( const wxString& dir ) -> bool
    {
        wxDir directory( dir );
        if( !directory.IsOpened() )
            return false;

        wxString filename;
        bool     cont = directory.GetFirst( &filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS );

        while( cont )
        {
            wxFileName filePath( dir, filename );

            if( filePath.FileExists()
                && ( filePath.GetExt() == "pro" || filePath.GetExt() == "kicad_pro" ) )
            {
                foundProPath = filePath.GetFullPath();
                return true;
            }
            else if( filePath.DirExists() )
            {
                if( FindProFile( filePath.GetFullPath() ) )
                    return true;
            }

            cont = directory.GetNext( &filename );
        }

        return false;
    };

    if( !FindProFile( unzipDir ) )
        return;

    // Step 5: Launch KiCad with .pro file
    wxString kicadCmd;

#ifdef __WXMSW__
    wxString kicadExe = appPath + wxFILE_SEP_PATH + "kicad.exe";
    if( !wxFileExists( kicadExe ) )
        kicadExe = "kicad.exe"; // fallback to PATH
#elif defined( __WXMAC__ )
    // Try to locate KiCad in Applications folder
    wxString macPath = "/Applications/KiCad/KiCad.app/Contents/MacOS/kicad";
    if( wxFileExists( macPath ) )
        kicadCmd = macPath;
    else
        kicadCmd = "kicad"; // fallback to PATH
#else
    kicadCmd = "kicad"; // Linux fallback to PATH
#endif

#ifdef __WXMSW__
    kicadCmd = "\"" + kicadExe + "\" \"" + foundProPath + "\"";
#else
    kicadCmd += " \"" + foundProPath + "\"";
#endif

    wxExecute( kicadCmd, wxEXEC_ASYNC );
}


#endif
