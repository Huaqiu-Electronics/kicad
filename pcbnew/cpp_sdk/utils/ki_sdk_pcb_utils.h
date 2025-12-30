#ifndef KI_SDK_PCB_UTILS_H
#define KI_SDK_PCB_UTILS_H

#include <pcb_edit_frame.h>
#include <board.h>
#include <cpp_sdk/ki_sdk_result_codes.h>
#include <wx/string.h>
#include <footprint_edit_frame.h>
#include <cpp_sdk/api_cpp_sdk_param.h>
#include <math/vector2d.h>

struct PcbUtilsContext
{
    PCB_EDIT_FRAME* frame = nullptr;
    BOARD* board = nullptr;

    bool IsValid() const { return frame && board; }
};



KI_SDK_EXEC_RESULT      GetPcbUtilsContext(PcbUtilsContext*   ctx);

KI_SDK_EXEC_RESULT      GetPcbEditFrame(PCB_EDIT_FRAME*&   frame);

KI_SDK_EXEC_RESULT      GetPcbBoard(BOARD*& board);


KI_SDK_EXEC_RESULT GetFootprintEditorFrame( FOOTPRINT_EDIT_FRAME*& frame );

int PcbMMToIU( const double& mm );

int PcbMilsToIU( const int& mils );

int PcbInchesToIU( const double& inch );


VECTOR2I PcbMMToIU( const API_POINT& pt );

double PcbIUToMM( const int& iu );

API_POINT PcbIUToMM( const VECTOR2I& pt);


#endif