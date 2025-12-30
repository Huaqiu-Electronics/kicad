#include <pcb_shape.h>
#include <board_design_settings.h>
#include <board_commit.h>
#include <widgets/appearance_controls.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_arc.h"

PCB_PRIMITIVES_ARC::PCB_PRIMITIVES_ARC( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_ARC::CreateArc( const VECTOR2I& center, const VECTOR2I& start, const VECTOR2I& end , std::string& aKID)
{
    const BOARD_DESIGN_SETTINGS& bds = GetBaseEditFrame()->GetDesignSettings();
    PCB_LAYER_ID                 layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }

    STROKE_PARAMS             stroke;

    stroke.SetWidth( bds.GetLineThickness( layer ) );
    stroke.SetLineStyle( LINE_STYLE::DEFAULT );
    stroke.SetColor( COLOR4D::UNSPECIFIED );

    
    PCB_SHAPE*  arc = new PCB_SHAPE( GetBoard() );
    arc->SetShape(SHAPE_T::ARC);
    arc->SetCenter(center);
    arc->SetFlags( IS_NEW );
    arc->SetStart( start );
    arc->SetEnd( end );
    arc->SetLayer( layer );
    arc->SetStroke( stroke );
    arc->SetFilled( false );

    GetBaseEditFrame()->SetMsgPanel( arc );


    aKID = arc->m_Uuid.AsStdString();

    GetBoard()->Add( arc );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( arc );
    GetBaseEditFrame()->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}