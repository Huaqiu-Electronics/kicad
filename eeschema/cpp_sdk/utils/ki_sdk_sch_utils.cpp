#include <wx/app.h>
#include <wx/log.h>
#include <eda_base_frame.h>
#include <kiway_holder.h>
#include <kiway.h>
#include "ki_sdk_sch_utils.h"
#include <pgm_base.h>
#include <cpp_sdk/api_cpp_sdk_param.h>


KI_SDK_EXEC_RESULT GetSymbolFrame( SYMBOL_EDIT_FRAME*& frame )
{
    KIWAY_PLAYER* editor;
    wxWindow*     kicadMgr_window = wxWindow::FindWindowByName( KICAD_MANAGER_FRAME_NAME );

    if( EDA_BASE_FRAME* kicadMgr = dynamic_cast<EDA_BASE_FRAME*>( kicadMgr_window ) )
    {
        editor = kicadMgr->Kiway().Player( FRAME_T::FRAME_SCH_SYMBOL_EDITOR, false );
    }
    else
    {
        frame = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    if( SYMBOL_EDIT_FRAME* current_frame = dynamic_cast<SYMBOL_EDIT_FRAME*>( editor ) )
    {
        frame = current_frame;
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }

    frame = nullptr;
    return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
}

KI_SDK_EXEC_RESULT GetSchEditorFrame( SCH_EDIT_FRAME*& frame )
{ 
    KIWAY_PLAYER* editor;
    wxWindow*     kicadMgr_window = wxWindow::FindWindowByName( KICAD_MANAGER_FRAME_NAME );

    if( EDA_BASE_FRAME* kicadMgr = dynamic_cast<EDA_BASE_FRAME*>( kicadMgr_window ) )
    {
        editor = kicadMgr->Kiway().Player( FRAME_T::FRAME_SCH, false );
    }
    else
    {
        frame = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    if( SCH_EDIT_FRAME* current_frame = dynamic_cast<SCH_EDIT_FRAME*>( editor ) )
    {
        frame = current_frame;
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }
    frame = nullptr;
    return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
}

KI_SDK_EXEC_RESULT GetSchematic( SCHEMATIC*& schematic )
{
    SCH_EDIT_FRAME*    frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;


    schematic = &( frame->Schematic() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

int SchMMToIU( const double& mm )
{
    return schIUScale.mmToIU( mm );
}

int SchMilsToIU( const int& mils )
{
    return schIUScale.MilsToIU( mils );
}

double SchIUToMM( const int& iu )
{
    return schIUScale.IUTomm( iu );
}

int SchIUToMils( const int& iu )
{
    return schIUScale.IUToMils( iu );
}

VECTOR2I SchMMToIU( const API_POINT& pt )
{
    return VECTOR2I( SchMMToIU( pt.x ), SchMMToIU( pt.y ) );
}

API_POINT SchIUToMM( const VECTOR2I& pt )
{
    return API_POINT  {SchIUToMM( pt.x ), SchIUToMM( pt.y ) };
}

KI_SDK_EXEC_RESULT GetGlobalSymbolDir( std::string& symbol_dir)
{
    const ENV_VAR_MAP& envVars = Pgm().GetLocalEnvVariables();

    for( auto it = envVars.begin(); it != envVars.end(); ++it )
    {
        if( it->first.Contains("SYMBOL"))
        {
            const wxString& path = it->second.GetValue();
            symbol_dir = path.ToStdString();
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}