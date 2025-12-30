#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include "sch_primitives_circle.h"
#include <sch_shape.h>
#include <sch_commit.h>
#include <pgm_base.h>
#include <symbol_editor/symbol_editor_settings.h>
#include <settings/settings_manager.h>
#include <symbol_editor/symbol_edit_frame.h>

SCH_PRIMITIVES_CIRCLE::SCH_PRIMITIVES_CIRCLE( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_CIRCLE::CreateCircle(const VECTOR2I& center, int radius, std::string& aKID)
{
    SCH_SHAPE* circle = new SCH_SHAPE( SHAPE_T::CIRCLE, GetLayerId(), 0, FILL_T::NO_FILL );
    VECTOR2I screenStart= center;
    VECTOR2I screenEnd = screenStart + VECTOR2I{0,radius};
    
    circle->SetFillColor(COLOR4D::UNSPECIFIED);
    circle->SetParent( GetItemParent() );
    circle->SetFlags( IS_NEW );
    circle->BeginEdit( screenStart );
    circle->CalcEdit(screenEnd);
    circle->EndEdit();
    circle->ClearEditFlags();

    aKID = circle->m_Uuid.AsStdString();

    AddItemToScreen( circle );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}