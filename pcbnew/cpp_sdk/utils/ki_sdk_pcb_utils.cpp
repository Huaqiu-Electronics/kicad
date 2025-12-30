#include <wx/app.h>
#include <wx/log.h>
#include <pgm_base.h>
#include <kiway.h>
#include <kiway_player.h>
#include <wx/defs.h>
#include <wx/window.h>
#include <kiway_holder.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include <cpp_sdk/api_cpp_sdk_param.h>


KI_SDK_EXEC_RESULT GetFootprintEditorFrame(FOOTPRINT_EDIT_FRAME*& frame)
{
    KIWAY_PLAYER* editor;
    wxWindow*     kicadMgr_window = wxWindow::FindWindowByName( KICAD_MANAGER_FRAME_NAME );
    if( EDA_BASE_FRAME* kicadMgr = dynamic_cast<EDA_BASE_FRAME*>( kicadMgr_window ) )
    {
        editor = kicadMgr->Kiway().Player( FRAME_T::FRAME_FOOTPRINT_EDITOR, false );
    }
    else
    {
        frame = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    frame = dynamic_cast<FOOTPRINT_EDIT_FRAME*>( editor );
    if( !frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT GetPcbUtilsContext( PcbUtilsContext*   ctx)
{
    KIWAY_PLAYER* editor;
    wxWindow* kicadMgr_window = wxWindow::FindWindowByName( KICAD_MANAGER_FRAME_NAME );
    
    if( EDA_BASE_FRAME* kicadMgr = dynamic_cast<EDA_BASE_FRAME*>(kicadMgr_window))
    {
        editor = kicadMgr->Kiway().Player(FRAME_T::FRAME_PCB_EDITOR, false);
    }
    else
    {
        ctx->frame = nullptr;
        ctx->board = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    ctx->frame = dynamic_cast<PCB_EDIT_FRAME*>(editor);
    if (!ctx->frame)
    {
        ctx->board = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    ctx->board = ctx->frame->GetBoard();
    if (!ctx->board)
    {
        ctx->frame = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_BOARD_ERROR;
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT GetPcbEditFrame(PCB_EDIT_FRAME*&   frame)
{
    PcbUtilsContext   ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);
    if(status == KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS)
    {
        frame = ctx.frame;
    }
    return status;
}

KI_SDK_EXEC_RESULT  GetPcbBoard(BOARD*& board)
{
    PcbUtilsContext   ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);
    if(status == KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS)
    {
        board = ctx.board;
    }
    return status;
}

int PcbMMToIU( const double& mm )
{
    return pcbIUScale.mmToIU( mm );
}

int PcbMilsToIU( const int& mils )
{
    return pcbIUScale.MilsToIU( mils );
}

int PcbInchesToIU( const double& inch )
{
    double mils = inch * 10e3;
    return pcbIUScale.MilsToIU( mils );
}

double PcbIUToMM( const int& iu )
{
    return pcbIUScale.IUTomm( iu );
}


VECTOR2I PcbMMToIU( const API_POINT& pt )
{
    return VECTOR2I( PcbMMToIU( pt.x ), PcbMMToIU( pt.y ) );
}

API_POINT PcbIUToMM( const VECTOR2I& pt)
{
    return API_POINT{PcbIUToMM( pt.x ), PcbIUToMM( pt.y ) };
}