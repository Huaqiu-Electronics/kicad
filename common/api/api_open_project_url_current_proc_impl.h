#ifndef API_OPEN_PROJECT_URL_CURRENT_PROC_IMPL_H
#define API_OPEN_PROJECT_URL_CURRENT_PROC_IMPL_H

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
#include <kiway_player.h>
#include <vector>


void UnZipAchieveToTMPDir( const std::string& aPrjZipUrl , wxFileName& outProjectPath)
{
    wxFileName  zipFn(aPrjZipUrl);
    wxFileName exeFile( wxStandardPaths::Get().GetExecutablePath() );
    wxString   appPath = exeFile.GetPath();

    const std::optional<std::string> zip_buffer =
            API_DESIGN_RESOURCE_MGR::DownloadURL( aPrjZipUrl );

    if( !zip_buffer )
        return;

    // Step 1: Write zip to tmp file
    wxString tempDir = wxStandardPaths::Get().GetTempDir();
    wxString zipPath = tempDir + wxFILE_SEP_PATH + zipFn.GetFullName();

    {
        wxFileOutputStream out( zipPath );
        out.Write( zip_buffer->data(), zip_buffer->size() );
    }

    // Step 2.1: Create unzip folder
    wxString unzipDir = tempDir + wxFILE_SEP_PATH + "unzipped_project";

    if(!wxFileName::DirExists(unzipDir))
    {
        wxMkdir( unzipDir, wxS_DIR_DEFAULT );
    }
    
    // Step 2.2: Create unzip project folder, "project.zip"->zipFn.GetName()
    wxString projectDir = unzipDir + wxFILE_SEP_PATH + zipFn.GetName();

    if(!wxFileName::DirExists(projectDir))
    {
        wxMkdir( projectDir, wxS_DIR_DEFAULT );
    }

    // Step 3.1: Extract zip
    {
        wxFFileInputStream          in( zipPath );
        wxZipInputStream            zipStream( in );
        std::unique_ptr<wxZipEntry> entry;

        while( ( entry.reset( zipStream.GetNextEntry() ), entry ) )
        {
            wxString   r_path = projectDir + wxFILE_SEP_PATH + entry->GetName();
            wxFileName entryPath( r_path );

            if( entry->IsDir() )
            {

                if(!wxFileName::DirExists(r_path))
                {
                    wxMkdir( entryPath.GetFullPath(), wxS_DIR_DEFAULT );
                }
                
            }
            else
            {
                //wxMkdir( entryPath.GetPath(), wxS_DIR_DEFAULT );
                wxFileOutputStream out( entryPath.GetFullPath() );
                out.Write( zipStream );
            }
        }
    }

    // Step 3.2 Remove zip
    wxRemoveFile(zipPath);

    // Step 4: Search .pro or .kicad_pro file recursively in the "projectDir"
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

    if( !FindProFile( projectDir ) )
        return;

    outProjectPath = wxFileName( foundProPath );
    return;
}


void OpenProjectLaunchKiCad( const std::string& aPrjZipUrl )
{
    wxFileName exeFile( wxStandardPaths::Get().GetExecutablePath() );
    wxString   appPath = exeFile.GetPath();
    wxFileName  project_file_path;
    //1-4 steps
    UnZipAchieveToTMPDir( aPrjZipUrl , project_file_path);

    if(!project_file_path.Exists())
    {
        wxLogError("Cannot find the .kicad_pro or .pro file");
        return;
    }

    wxString    foundProPath = project_file_path.GetFullPath();

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

void OpenProjectInCurrentProcess(const std::string& aPrjZipUrl, KIWAY_PLAYER* kiway_player)
{
    wxFileName  project_file_path;
    //1-4 steps
    UnZipAchieveToTMPDir( aPrjZipUrl , project_file_path);

    if(!project_file_path.Exists())
    {
        wxLogError("Cannot find the .kicad_pro or .pro file");
        return;
    }

    wxString extension;
    wxString legacy_extension;

    if(FRAME_T::FRAME_PCB_EDITOR == kiway_player->GetFrameType())
    {
        extension = FILEEXT::KiCadPcbFileExtension;
        legacy_extension = FILEEXT::LegacyPcbFileExtension;
    }
    else if(FRAME_T::FRAME_SCH == kiway_player->GetFrameType())
    {
        extension = FILEEXT::KiCadSchematicFileExtension;
        legacy_extension = FILEEXT::LegacySchematicFileExtension;
    }else
    {
        wxLogError("Current Frame type do not support");
        return;
    }

    wxFileName  sch_file_path(project_file_path.GetPath(), project_file_path.GetName(), extension);//FILEEXT::KiCadSchematicFileExtension

    if(!sch_file_path.Exists())
    {
        sch_file_path.SetExt(legacy_extension);

        if(!sch_file_path.Exists())
        {
            wxLogError("Can not File .%s file or .%s in the project zip achieve.",  extension, legacy_extension);
            return;
        }
        
    }   
    
    // Step 5: Open .kicad_sch .sch .kicad_pro .brd in Current Kicad.
    kiway_player->OpenProjectFiles( std::vector<wxString>( 1, sch_file_path.GetFullPath() ) );
}


#endif