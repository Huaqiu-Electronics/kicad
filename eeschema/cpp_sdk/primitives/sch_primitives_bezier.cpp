#include "sch_primitives_bezier.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_shape.h>


SCH_PRIMITIVES_BEZIER::SCH_PRIMITIVES_BEZIER( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{

}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_BEZIER::CreateBezier(const VECTOR2I& start, const VECTOR2I& c1, const VECTOR2I& c2, const VECTOR2I& end, std::string& aKID)
{
    SCH_SHAPE* bezier = new SCH_SHAPE( SHAPE_T::BEZIER, GetLayerId(), 0, FILL_T::NO_FILL );

    bezier->SetFillColor(COLOR4D::UNSPECIFIED);
    bezier->SetParent( GetItemParent() );
    bezier->SetFlags( IS_NEW );
    bezier->BeginEdit( start );

    bezier->ContinueEdit(c1);
    bezier->ContinueEdit(c1);
    bezier->SetBezierC1(c1);
    bezier->SetBezierC2(c2);
    bezier->SetEnd(end);
    bezier->CalcEdit(c2);
    bezier->EndEdit();
    bezier->ClearEditFlags();
    bezier->SetFlags( IS_NEW );

    aKID = bezier->m_Uuid.AsStdString();

    AddItemToScreen( bezier );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}