#include "sch_primitives_schematic_text.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_text.h>

SCH_PRIMITIVES_SCHEMATIC_TEXT::SCH_PRIMITIVES_SCHEMATIC_TEXT( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{

}



KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SCHEMATIC_TEXT::CreateSchematicText(const VECTOR2I& position, const std::string& text, std::string& aKID)
{
    wxString  text_value = wxString::FromUTF8(text);
    SCH_TEXT*  sch_text = nullptr;

    sch_text = new SCH_TEXT( position );
    sch_text->SetParent( GetItemParent() );
    sch_text->SetBold( false );
    sch_text->SetItalic( false );
    sch_text->SetHorizJustify( GR_TEXT_H_ALIGN_CENTER );
    sch_text->SetVertJustify( GR_TEXT_V_ALIGN_CENTER );
    sch_text->SetTextAngle( ANGLE_0 );
    sch_text->SetFlags( IS_NEW );
    sch_text->SetText( text_value );

    aKID = sch_text->m_Uuid.AsStdString();

    AddItemToScreen( sch_text );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}