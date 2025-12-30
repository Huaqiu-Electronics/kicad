#include <pcb_shape.h>
#include <board_design_settings.h>
#include <board_commit.h>
#include <widgets/appearance_controls.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_circle.h"


PCB_PRIMITIVES_CIRCLE::PCB_PRIMITIVES_CIRCLE( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_CIRCLE::CreateCircle(const VECTOR2I& center, int radius, std::string& aKID)
{
    const BOARD_DESIGN_SETTINGS& bds = GetBaseEditFrame()->GetDesignSettings();
    PCB_LAYER_ID                 layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }
    STROKE_PARAMS             stroke;

    layer = GetBaseEditFrame()->GetActiveLayer();
    stroke.SetWidth( bds.GetLineThickness( layer ) );
    stroke.SetLineStyle( LINE_STYLE::DEFAULT );
    stroke.SetColor( COLOR4D::UNSPECIFIED );

    PCB_SHAPE*  circle = new PCB_SHAPE( GetBoard() );
    circle->SetShape(SHAPE_T::CIRCLE);
    circle->SetFlags( IS_NEW );
    circle->SetCenter( VECTOR2I(center.x, center.y) );
    circle->SetRadius( radius );
    circle->SetLayer( layer );
    circle->SetStroke( stroke );
    circle->SetFilled( false );

    GetBaseEditFrame()->SetMsgPanel( circle );
    aKID = circle->m_Uuid.AsStdString();

    GetBoard()->Add( circle );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( circle );
    GetBaseEditFrame()->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}