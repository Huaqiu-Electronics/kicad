#include "sch_primitives_rectangle.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_shape.h>

SCH_PRIMITIVES_RECTANGLE::SCH_PRIMITIVES_RECTANGLE( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{
}

KI_SDK_EXEC_RESULT  SCH_PRIMITIVES_RECTANGLE::CreateRectangle(const VECTOR2I& pos, int width, int height,  std::string& aKID)
{
    SCH_SHAPE*  rectangle = new SCH_SHAPE( SHAPE_T::RECTANGLE, GetLayerId(), 0, FILL_T::NO_FILL );
    VECTOR2I    screenStartPos = pos;
    VECTOR2I    screenEndPos = screenStartPos + VECTOR2I{width,height};

    rectangle->SetFillColor(COLOR4D::UNSPECIFIED);
    rectangle->SetParent( GetItemParent() );
    rectangle->SetFlags( IS_NEW );
    rectangle->BeginEdit( screenStartPos );
    rectangle->CalcEdit(screenEndPos);
    rectangle->EndEdit();
    rectangle->ClearEditFlags();
    rectangle->SetFlags( IS_NEW );

    aKID = rectangle->m_Uuid.AsStdString();
    AddItemToScreen( rectangle );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}