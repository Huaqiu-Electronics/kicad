#include "sch_primitives_label.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_label.h>

KI_SDK_EXEC_RESULT FindLabelByKIID( const std::string& aKID, SCH_LABEL*& aLabel )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    for( auto& elem : frame->GetScreen()->Items().OfType( SCH_LABEL_T ) )
    {
        if( elem->m_Uuid.AsStdString() == aKID )
        {
            aLabel = static_cast<SCH_LABEL*>( elem );
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::CreateCommonLabel( const VECTOR2I& position, const std::string& text,
                                                            std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCHEMATIC_SETTINGS& settings = schematic->Settings();

    SCH_LABEL* label = new SCH_LABEL( position );

    label->SetParent( schematic );
    label->SetTextSize( VECTOR2I( settings.m_DefaultTextSize, settings.m_DefaultTextSize ) );
    label->SetSpinStyle( SPIN_STYLE::RIGHT );

    wxString text_value = wxString::FromUTF8( text );
    label->SetText( text_value );

    // label->SetFlags(IS_NEW);

    label->SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
    label->SetVertJustify( GR_TEXT_V_ALIGN_BOTTOM );

    //m_selectionTool->AddItemToSel(labelItem);

    label->ClearFlags();

    aKID = label->m_Uuid.AsStdString();

    frame->AddToScreen( label, frame->GetScreen() );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::CreateClassLabel( const VECTOR2I& position, const std::string& aNetClass,
                                                           const std::string& aComponentClass, std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCHEMATIC_SETTINGS&  settings = schematic->Settings();
    SCH_DIRECTIVE_LABEL* label = new SCH_DIRECTIVE_LABEL( position );
    label->SetShape( LABEL_FLAG_SHAPE::F_ROUND );
    // FIXME
    // label->GetFields().emplace_back( SCH_FIELD( { 0, 0 }, 0, label, wxT( "Netclass" ) ) );
    // label->GetFields().emplace_back( SCH_FIELD( { 0, 0 }, 0, label, wxT( "Component Class" ) ) );
    label->GetFields().back().SetItalic( true );
    label->GetFields().back().SetVisible( true );

    label->SetParent( schematic );
    label->SetTextSize( VECTOR2I( settings.m_DefaultTextSize, settings.m_DefaultTextSize ) );
    label->SetSpinStyle( SPIN_STYLE::RIGHT );

    wxString net_class = wxString::FromUTF8( aNetClass );
    wxString component_class = wxString::FromUTF8( aComponentClass );
    label->GetFields().at( 0 ).SetText( net_class );
    label->GetFields().at( 0 ).SetVisible( true );

    label->GetFields().at( 1 ).SetText( aComponentClass );
    label->GetFields().at( 1 ).SetVisible( true );
    //labelItem->SetText(textvalue);

    // label->SetFlags(IS_NEW );

    label->SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
    label->SetVertJustify( GR_TEXT_V_ALIGN_BOTTOM );
    label->AutoplaceFields( nullptr, AUTOPLACE_AUTO );

    label->SetPosition( position );

    label->ClearFlags();

    aKID = label->m_Uuid.AsStdString();

    frame->AddToScreen( label, frame->GetScreen() );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::CreateGlobalLabel( const VECTOR2I& position, const std::string& text,
                                                            std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCHEMATIC_SETTINGS& settings = schematic->Settings();
    SCH_GLOBALLABEL*    label = new SCH_GLOBALLABEL( position );
    label->SetShape( LABEL_FLAG_SHAPE::L_INPUT );
    label->GetFields()[0].SetVisible( settings.m_IntersheetRefsShow );
    label->SetAutoRotateOnPlacement( false );

    label->SetParent( schematic );
    label->SetTextSize( VECTOR2I( settings.m_DefaultTextSize, settings.m_DefaultTextSize ) );
    label->SetSpinStyle( SPIN_STYLE::RIGHT );

    wxString text_value = wxString::FromUTF8( text );
    label->SetText( text_value );

    label->SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
    label->SetVertJustify( GR_TEXT_V_ALIGN_CENTER );

    label->ClearFlags();
    aKID = label->m_Uuid.AsStdString();
    frame->AddToScreen( label, frame->GetScreen() );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::CreateHierLabel( const VECTOR2I& position, const std::string& text,
                                                          std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCHEMATIC_SETTINGS& settings = schematic->Settings();
    SCH_LABEL_BASE*     label = new SCH_HIERLABEL( position );
    label->SetShape( LABEL_FLAG_SHAPE::L_INPUT );
    label->SetAutoRotateOnPlacement( false );

    label->SetParent( schematic );
    label->SetTextSize( VECTOR2I( settings.m_DefaultTextSize, settings.m_DefaultTextSize ) );

    label->SetSpinStyle( SPIN_STYLE::RIGHT );
    // label->SetFlags(IS_NEW);

    wxString text_value = wxString::FromUTF8( text );
    label->SetText( text_value );

    label->SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
    label->SetVertJustify( GR_TEXT_V_ALIGN_CENTER );

    label->ClearFlags();

    aKID = label->m_Uuid.AsStdString();
    frame->AddToScreen( label, frame->GetScreen() );


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::RotateLabel( const std::string& aKID, int angle )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_LABEL* sch_label = nullptr;

    FindLabelByKIID( aKID, sch_label );

    if( nullptr == sch_label )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    if( angle == 90 )
    {
        sch_label->Rotate90( true );
    }
    else if( angle == 270 )
    {
        sch_label->Rotate90( false );
    }
    else if( angle == 180 )
    {
        sch_label->Rotate90( true );
        sch_label->Rotate90( true );
    }

    frame->UpdateItem( sch_label );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LABEL::MoveLabel( const std::string& aKID, const VECTOR2I& aOffset )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_LABEL* sch_label = nullptr;
    FindLabelByKIID( aKID, sch_label );

    if( nullptr == sch_label )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_label->Move( aOffset );

    frame->UpdateItem( sch_label );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}
