#include "sch_primitives_arc.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_shape.h>

SCH_PRIMITIVES_ARC::SCH_PRIMITIVES_ARC( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_ARC::CreateArc(const VECTOR2I& start, const VECTOR2I& end, std::string& aKID)
{
    COLOR4D    fillColor(COLOR4D::UNSPECIFIED);
    SCH_SHAPE* arc = new SCH_SHAPE( SHAPE_T::ARC, GetLayerId(), 0, FILL_T::NO_FILL );

    arc->SetFillColor(COLOR4D::UNSPECIFIED);
    arc->SetParent( GetItemParent() );
    arc->SetFlags( IS_NEW );
    arc->BeginEdit( start );
    arc->CalcEdit(end);
    arc->EndEdit();
    arc->ClearEditFlags();

    aKID = arc->m_Uuid.AsStdString();
    AddItemToScreen( arc );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}