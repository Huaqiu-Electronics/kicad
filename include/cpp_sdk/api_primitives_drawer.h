#ifndef API_PRIMITIVES_DRAWER_H
#define API_PRIMITIVES_DRAWER_H

#include "api_cpp_sdk_param.h"
#include <eda_draw_frame.h>
#include <magic_enum.hpp>
#include <kiway.h>
#include <wx/wx.h>
#include <vector>
#include <string>
#include <wildcards_and_files_ext.h>
#include <tool/actions.h>
#include <tool/tool_manager.h>
class API_PRIMITIVES_DRAWER
{
public:
    explicit API_PRIMITIVES_DRAWER( EDA_DRAW_FRAME* aFrame ) : m_frame( aFrame ) {}

public:
    FRAME_T GetFrameType() { return m_frame->GetFrameType(); }

    void CloseFrame( API_FRAME_PARAMS const& aParams )
    {
        auto frame_type = magic_enum::enum_cast<FRAME_T>( aParams.frame_type );

        if( !frame_type )
            return;

        KIWAY_PLAYER* frame = m_frame->Kiway().Player( *frame_type, false );

        if( frame )
        {
            frame->Close();
        }

        return;
    }

    void GetCurrentFrame( API_FRAME_PARAMS& aParams )
    {
        auto frame_type = m_frame->GetFrameType();
        aParams.frame_type = magic_enum::enum_name( frame_type );
        return;
    }

    void OpenFrame( API_FRAME_PARAMS const& aParams )
    {
        auto frame_type = magic_enum::enum_cast<FRAME_T>( aParams.frame_type );

        if( !frame_type )
            return;

        wxString              msg;
        wxFileName            project_file_path;
        std::vector<wxString> load_file_list;
        bool load_proj = false;

        if( *frame_type == FRAME_T::FRAME_SCH || *frame_type == FRAME_T::FRAME_PCB_EDITOR )
        {
            load_proj = true;
        }

        //get pcb board file path
        if( load_proj && ( frame_type == FRAME_PCB_EDITOR ) )
        {
            wxFileName kicad_board( m_frame->Prj().GetProjectPath(), m_frame->Prj().GetProjectName(),
                                    FILEEXT::KiCadPcbFileExtension );

            if( !kicad_board.FileExists() )
            {
                wxFileName legacy_board( m_frame->Prj().GetProjectPath(), m_frame->Prj().GetProjectName(),
                                         FILEEXT::LegacyPcbFileExtension );

                if( legacy_board.FileExists() )
                {
                    kicad_board = legacy_board;
                }
                else
                {
                    msg.Printf( _( "PCB Editor board file '%s' not found." ), kicad_board.GetFullPath() );
                    wxLogError( msg );
                    return;
                }
            }

            project_file_path = kicad_board;
            load_file_list.push_back( project_file_path.GetFullPath() );
        }

        if( load_proj && ( frame_type == FRAME_SCH ) )
        {
            wxFileName schematic( m_frame->Prj().GetProjectPath(), m_frame->Prj().GetProjectName(),
                                  FILEEXT::KiCadSchematicFileExtension );

            if( !schematic.FileExists() )
            {
                wxFileName legacySchematic( m_frame->Prj().GetProjectPath(), m_frame->Prj().GetProjectName(),
                                            FILEEXT::LegacySchematicFileExtension );

                if( legacySchematic.FileExists() )
                {
                    schematic = legacySchematic;
                }
                else
                {
                    msg.Printf( _( "Schematic file '%s' not found." ), schematic.GetFullPath() );
                    wxLogError( msg );
                    return;
                }
            }
            project_file_path = schematic;
            load_file_list.push_back( project_file_path.GetFullPath() );
        }

        KIWAY_PLAYER* frame = m_frame->Kiway().Player( *frame_type, false );

        if( !frame )
        {
            try
            {
                frame = m_frame->Kiway().Player( *frame_type, true );
            }
            catch( std::exception& err )
            {
                msg.Printf( _( "Editor failed to load : '%s'." ), err.what() );
                wxLogError( msg );
                return;
            }
        }

        if( !frame )
        {
            wxLogError( "Failed Open Editor." );
            return;
        }

        if( !frame->IsShownOnScreen() )
        {
            if( load_proj && !load_file_list.empty() )
            {
                frame->OpenProjectFiles( load_file_list );
            }
            frame->Show( true );
        }

        if( frame->IsIconized() )
        {
            frame->Iconize( false );
            frame->Maximize( false );
        }

        frame->Raise();
    }


    void SaveFrame() { m_frame->GetToolManager()->RunAction( ACTIONS::save ); }

    void SaveAs() { m_frame->GetToolManager()->RunAction( ACTIONS::saveAs ); }

    void OpenPageSettingDlg(){ m_frame->GetToolManager()->RunAction( ACTIONS::pageSettings ); }

    void OpenPrintDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::print ); }

    void OpenPlotDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::plot ); }

    void CloseCurrentFrame()
    {
        wxCommandEvent menuEvent( wxEVT_MENU, wxID_CLOSE );
        m_frame->GetEventHandler()->ProcessEvent( menuEvent );
    }

    void OpenFindDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::find ); }

    void OpenFindReplaceDlg() { m_frame->GetToolManager()->RunAction(ACTIONS::findAndReplace); }

    void DeleteItem() { m_frame->GetToolManager()->RunAction( ACTIONS::deleteTool);}

    void SelectAllItems() { m_frame->GetToolManager()->RunAction( ACTIONS::selectAll ); }

    void UnSelectAllItems() { m_frame->GetToolManager()->RunAction( ACTIONS::unselectAll); }

    void ShowPropertyPanel() { m_frame->GetToolManager()->RunAction( ACTIONS::showProperties);}

    void ShowSearchPanel() { m_frame->GetToolManager()->RunAction( ACTIONS::showSearch);}

    void ZoomInCenter() { m_frame->GetToolManager()->RunAction( ACTIONS::zoomInCenter); }

    void ZoomOutCenter() { m_frame->GetToolManager()->RunAction( ACTIONS::zoomOutCenter); }

    void ZoomFitScreen() { m_frame->GetToolManager()->RunAction( ACTIONS::zoomFitScreen); }

    void ZoomFitObjects() { m_frame->GetToolManager()->RunAction( ACTIONS::zoomFitObjects ); }

    void OpenConfigurePathDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::configurePaths ); }

    void OpenSymbolLibDlg(){ m_frame->GetToolManager()->RunAction( ACTIONS::showSymbolLibTable); }

    void OpenDesignBlockLibDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::showDesignBlockLibTable ); }

    void OpenPreferencesDlg() { m_frame->GetToolManager()->RunAction( ACTIONS::openPreferences ); }

protected:
    EDA_DRAW_FRAME* m_frame;
};


#endif