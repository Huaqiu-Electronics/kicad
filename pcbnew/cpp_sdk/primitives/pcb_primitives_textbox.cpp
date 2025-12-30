#include <pcb_text.h>
#include <pcb_textbox.h>
#include <board_design_settings.h>
#include <widgets/appearance_controls.h>
#include <board_commit.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_textbox.h"


PCB_PRIMITIVES_TEXTBOX::PCB_PRIMITIVES_TEXTBOX( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_TEXTBOX::CreateTextBox( const VECTOR2I& position, int width, int height, const std::string& text, std::string& aKID )
{
    const BOARD_DESIGN_SETTINGS& bds = GetBaseEditFrame()->GetDesignSettings();
    PCB_LAYER_ID                 layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }

    PCB_TEXTBOX*  rect = new PCB_TEXTBOX( GetBoard());

    rect->SetShape(SHAPE_T::RECTANGLE);
    rect->SetFilled(false);
    rect->SetFlags( IS_NEW );

    rect->SetStart( position );
    rect->SetEnd( VECTOR2I(position.x + width, position.y + height) );
    rect->SetLayer( layer );
    STROKE_PARAMS             stroke;
    stroke.SetWidth( bds.GetLineThickness( layer ) );
    stroke.SetLineStyle( LINE_STYLE::DEFAULT );
    stroke.SetColor( COLOR4D::UNSPECIFIED );
    rect->SetStroke( stroke );  

    TEXT_ATTRIBUTES   textAttrs;
    textAttrs.m_Size = bds.GetTextSize( layer);     
    textAttrs.m_StoredStrokeWidth = bds.GetTextThickness( layer );
    textAttrs.m_Bold = false;
    textAttrs.m_Italic = bds.GetTextItalic( layer );
    textAttrs.m_KeepUpright = bds.GetTextUpright( layer );
    textAttrs.m_Mirrored = IsBackLayer( layer );    

    textAttrs.m_Halign = GR_TEXT_H_ALIGN_LEFT;
    textAttrs.m_Valign = GR_TEXT_V_ALIGN_TOP;

    rect->SetText( wxString::FromUTF8(text) );
    rect->SetAttributes( textAttrs );

    aKID = rect->m_Uuid.AsStdString();


    rect->Normalize();

    GetBoard()->Add( rect );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( rect );
    GetBaseEditFrame()->GetCanvas()->Refresh();

    // commit.Add( rect );
    // commit.Push(_("Draw Text Box"));

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_TEXTBOX::CreateText( const VECTOR2I& position, const std::string& text, std::string& aKID )
{
    const BOARD_DESIGN_SETTINGS& bds = GetBaseEditFrame()->GetDesignSettings();
    PCB_LAYER_ID                 layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }


    PCB_TEXT* label = new PCB_TEXT( GetBoard() );

    TEXT_ATTRIBUTES              textAttrs;
    textAttrs.m_Size = bds.GetTextSize( layer );
    textAttrs.m_StoredStrokeWidth = bds.GetTextThickness( layer );
    textAttrs.m_Bold = false;
    textAttrs.m_Italic = bds.GetTextItalic( layer );
    textAttrs.m_KeepUpright = bds.GetTextUpright( layer );
    textAttrs.m_Mirrored = IsBackLayer( layer );
    textAttrs.m_Halign = GR_TEXT_H_ALIGN_LEFT;
    textAttrs.m_Valign = GR_TEXT_V_ALIGN_BOTTOM;


    label->SetLayer( layer );
    label->SetText( wxString::FromUTF8( text ) );
    label->SetAttributes( textAttrs );
    label->SetTextPos( position );
    label->SetFlags( IS_NEW );

    label->ClearFlags();

    label->Normalize();

    GetBoard()->Add( label );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( label );
    GetBaseEditFrame()->GetCanvas()->Refresh();
    aKID = label->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}