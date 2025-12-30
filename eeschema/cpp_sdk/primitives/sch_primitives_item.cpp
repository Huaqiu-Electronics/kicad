#include <sch_base_frame.h>
#include <sch_edit_frame.h>
#include <symbol_editor/symbol_edit_frame.h>
#include <layer_ids.h>
#include "sch_primitives_item.h"

SCH_PRIMITIVES_ITEM::SCH_PRIMITIVES_ITEM( SCH_BASE_FRAME* aFrame ) : m_frame( aFrame ), m_parent( nullptr )
{
    wxASSERT( m_frame );
    initItemParent();
}

SCH_PRIMITIVES_ITEM::~SCH_PRIMITIVES_ITEM()
{
}

void SCH_PRIMITIVES_ITEM::initItemParent()
{
    if( m_frame )
    {
        FRAME_T frame_type = m_frame->GetFrameType();

        if( FRAME_SCH == frame_type )
        {
            SCH_EDIT_FRAME* frame = dynamic_cast<SCH_EDIT_FRAME*>( m_frame );
            if( frame )
            {
                m_parent = &( frame->Schematic() );
                SetLayerId( SCH_LAYER_ID::LAYER_NOTES );
            }
        }
        else if( FRAME_SCH_SYMBOL_EDITOR == frame_type )
        {
            SYMBOL_EDIT_FRAME* frame = dynamic_cast<SYMBOL_EDIT_FRAME*>( m_frame );
            if( frame )
            {
                m_parent = frame->GetCurSymbol();
                SetLayerId( SCH_LAYER_ID::LAYER_DEVICE );
            }
        }
    }
}


FRAME_T SCH_PRIMITIVES_ITEM::GetFrameType() const
{
    return m_frame->GetFrameType();
}

void SCH_PRIMITIVES_ITEM::AddItemToScreen( SCH_ITEM* item )
{
    if( m_frame )
    {
        if( FRAME_SCH == GetFrameType() )
        {
            SCH_EDIT_FRAME* frame = dynamic_cast<SCH_EDIT_FRAME*>( m_frame );
            if( frame )
            {
                frame->AddToScreen( item );
                frame->GetCanvas()->Refresh();
            }
        }
        else if( FRAME_SCH_SYMBOL_EDITOR == GetFrameType() )
        {
            SYMBOL_EDIT_FRAME* frame = dynamic_cast<SYMBOL_EDIT_FRAME*>( m_frame );
            if( frame )
            {
                frame->GetCurSymbol()->AddDrawItem( item );

                frame->GetCanvas()->GetView()->Add( item );
                frame->GetCanvas()->GetView()->Update( frame->GetCurSymbol() );
                frame->GetCanvas()->Refresh();
            }
        }
    }
}