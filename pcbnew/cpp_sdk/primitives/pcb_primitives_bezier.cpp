#include <pcb_shape.h>
#include <board_design_settings.h>
#include <board_commit.h>
#include <widgets/appearance_controls.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_bezier.h"


PCB_PRIMITIVES_BEZIER::PCB_PRIMITIVES_BEZIER( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_BEZIER::CreateBezier(const VECTOR2I& start, const VECTOR2I& c1, const VECTOR2I& c2, const VECTOR2I& end, std::string& aKID)
{
    PCB_LAYER_ID layer;
    layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }
    
    STROKE_PARAMS stroke;
    stroke.SetWidth( GetBaseEditFrame()->GetDesignSettings().GetLineThickness( layer ) );
    stroke.SetLineStyle( LINE_STYLE::DEFAULT );
    stroke.SetColor( COLOR4D::UNSPECIFIED );


    int maxError = GetBoard()->GetDesignSettings().m_MaxError;

    // BOARD_COMMIT commit( GetBaseEditFrame() );

    PCB_SHAPE* bezier = new PCB_SHAPE( GetBoard() );

    bezier->SetShape( SHAPE_T::BEZIER );
    bezier->SetFlags( IS_NEW );
    bezier->SetLayer( layer );
    bezier->SetStroke( stroke );
    bezier->SetStart( start );
    bezier->SetEnd( end );
    bezier->SetBezierC1( c1 );
    bezier->SetBezierC2( c2 );
    bezier->RebuildBezierToSegmentsPointsList( maxError );


    aKID = bezier->m_Uuid.AsStdString();

    GetBoard()->Add( bezier );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( bezier );
    GetBaseEditFrame()->GetCanvas()->Refresh();

    // GetBaseEditFrame()->SetMsgPanel( bezier );

    // commit.Add( bezier );
    // commit.Push( _( "Draw Bezier" ) );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}