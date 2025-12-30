#include <pcb_shape.h>
#include <board_design_settings.h>
#include <board_commit.h>
#include <widgets/appearance_controls.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_rectangle.h"
#include <pcb_edit_frame.h>
#include <footprint_edit_frame.h>

PCB_PRIMITIVES_RECTANGLE::PCB_PRIMITIVES_RECTANGLE( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_RECTANGLE::CreateRectangle( const VECTOR2I& position, int width, int height , std::string& aKID)
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


    PCB_SHAPE*  rectangle = new PCB_SHAPE( GetBoard());
    rectangle->SetShape(SHAPE_T::RECTANGLE);
    rectangle->SetFlags( IS_NEW );
    rectangle->SetStart( VECTOR2I(position.x, position.y) );
    rectangle->SetEnd( VECTOR2I(position.x + width, position.y + height) );
    rectangle->SetLayer( layer );
    rectangle->SetStroke( stroke );
    rectangle->SetFilled( false );


    aKID = rectangle->m_Uuid.AsStdString();

    GetBoard()->Add( rectangle );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( rectangle );
    GetBaseEditFrame()->GetCanvas()->Refresh();



    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}