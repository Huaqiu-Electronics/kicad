#include "pcb_primitives_item.h"
#include <pcb_edit_frame.h>
#include <footprint_edit_frame.h>
#include <cpp_sdk/ki_sdk_result_codes.h>


PCB_PRIMITIVES_ITEM::PCB_PRIMITIVES_ITEM( PCB_BASE_EDIT_FRAME* aFrame ) :
        m_base_frame( aFrame ),
        m_board( nullptr )
{
    if( m_base_frame )
    {
        m_board = m_base_frame->GetBoard();
    }
    
}

PCB_PRIMITIVES_ITEM::~PCB_PRIMITIVES_ITEM()
{

}


FRAME_T PCB_PRIMITIVES_ITEM::GetFrameType() const
{
    return m_base_frame->GetFrameType();
}
              