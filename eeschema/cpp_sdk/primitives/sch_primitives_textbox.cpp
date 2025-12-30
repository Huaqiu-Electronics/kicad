#include "sch_primitives_textbox.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_textbox.h>

SCH_PRIMITIVES_TEXTBOX::SCH_PRIMITIVES_TEXTBOX( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_TEXTBOX::CreateTextBox(const VECTOR2I& position, int width, int height, const std::string& text, std::string& aKID)
{
    wxString  textvalue = wxString::FromUTF8(text);

    SCH_TEXTBOX* textbox = new SCH_TEXTBOX(LAYER_NOTES, 0, FILL_T::NO_FILL);
    
    textbox->SetBold(false);
    textbox->SetItalic(false);

    textbox->SetTextAngle( ANGLE_0 );
    textbox->SetHorizJustify( GR_TEXT_H_ALIGN_CENTER );

    textbox->SetVertJustify( GR_TEXT_V_ALIGN_CENTER );
    textbox->SetStroke( STROKE_PARAMS{} );

    textbox->SetFillColor( COLOR4D::UNSPECIFIED );
    textbox->SetParent( GetItemParent() );

    textbox->SetFlags( IS_NEW );

    textbox->BeginEdit( position );


    VECTOR2I endPoint = position + VECTOR2I{width, height};
    textbox->CalcEdit(endPoint);
    textbox->EndEdit();
    textbox->ClearEditFlags();
    textbox->SetFlags(IS_NEW);

    textbox->SetText(textvalue);

    aKID = textbox->m_Uuid.AsStdString();

    AddItemToScreen( textbox );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}