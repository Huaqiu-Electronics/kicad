#include "sch_primitives_symbol.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_symbol.h>
#include <io/io_mgr.h>
#include <sch_io/sch_io.h>
#include <math/box2.h>
// #include <symbol_lib_table.h>
#include "sch_primitives_line.h"
#include "sch_primitives_label.h"
#include <project_sch.h>
#include <symbol_editor/lib_symbol_library_manager.h>
#include <kiway.h>
#include <mail_type.h>
#include <sch_pin.h>
#include <pin_type.h>
#include "sch_primitives_marker.h"
#include <sch_line.h>
#include <sch_marker.h>
// #include <symbol_lib_table.h>
#include <libraries/library_manager.h>
#include <libraries/symbol_library_adapter.h>
#include <libraries/library_table.h>
#include <kiway_holder.h>

KI_SDK_EXEC_RESULT FindSymbolByKIID( const std::string& aKID, SCH_SYMBOL*& aSymbol )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_REFERENCE_LIST references;
    frame->Schematic().Hierarchy().GetSymbols( references, {} );
    for( size_t i = 0; i < references.GetCount(); i++ )
    {
        SCH_SYMBOL* item = references[i].GetSymbol();
        if( item->m_Uuid.AsStdString() == aKID )
        {
            aSymbol = static_cast<SCH_SYMBOL*>( item );
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT FindSymbolPinByNumber( const SCH_SYMBOL* aSymbol, const std::string& aNumber, SCH_PIN*& aPin )
{
    if( nullptr == aSymbol )
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;

    for( auto& sch_pin : aSymbol->GetPins() )
    {
        if( sch_pin->GetNumber().ToStdString() == aNumber )
        {
            aPin = sch_pin;
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}



KI_SDK_EXEC_RESULT GetSymbolSheetByKIID( const std::string& aKID, SCH_SHEET_PATH& aSheetPath )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    // SCH_SYMBOL* sch_symbol = nullptr;
    // FindSymbolByKIID( aKID, sch_symbol );

    // if( nullptr == sch_symbol )
    //     return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    SCH_REFERENCE_LIST references;
    frame->Schematic().Hierarchy().GetSymbols( references , {} );
    for( size_t i = 0; i < references.GetCount(); i++ )
    {
        SCH_SYMBOL* item = references[i].GetSymbol();
        if( item->m_Uuid.AsStdString() == aKID )
        {
            // aSymbol = static_cast<SCH_SYMBOL*>( item );
            aSheetPath = references[i].GetSheetPath();
            // aSheetIndex = curr_sheetpath->GetPageNumber();
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::PlaceSymbol( const wxString& aFileName, const wxString& symbol_nick_name,
                                                        const VECTOR2I& position, std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCHEMATIC* schematic = nullptr;
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = nullptr;
    // frame->GetLibSymbol()

    try
    {
        IO_RELEASER<SCH_IO> pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_KICAD ) );
        LIB_SYMBOL*         asymbol = pi->LoadSymbol( aFileName, symbol_nick_name );
        if( asymbol )
        {
            LIB_ID lib_id = asymbol->GetLibId();
            lib_id.SetLibNickname( symbol_nick_name );
            asymbol->SetLibId( lib_id );
            lib_symbol = new LIB_SYMBOL( *asymbol );
        }
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Failed to load library %s. %s" ), aFileName, ioe.What() );
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }
    if( nullptr == lib_symbol )
    {
        wxLogError( _( "Failed to load library %s." ), aFileName );
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    PICKED_SYMBOL pickedSymbol;
    pickedSymbol.LibId = lib_symbol->GetLIB_ID();

    SCH_SYMBOL* symbol = new SCH_SYMBOL( *lib_symbol, &frame->GetCurrentSheet(), pickedSymbol, position, schematic );
    aKID = symbol->m_Uuid.AsStdString();
    frame->AddToScreen( symbol );
    frame->GetCanvas()->Refresh();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetAllSymbolKIID( std::vector<std::string>& allKIDList )
{
    allKIDList.clear();
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );
    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    for( auto& elem : frame->GetScreen()->Items().OfType( SCH_SYMBOL_T ) )
    {
        allKIDList.emplace_back( elem->m_Uuid.AsStdString() );
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolReferenceByKIID( const std::string& aKID, std::string& aReference )
{
    SCH_SYMBOL* aSymbol = nullptr;
    FindSymbolByKIID( aKID, aSymbol );

    if( nullptr == aSymbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // aReference = aSymbol->GetField( REFERENCE_FIELD )->GetText().ToStdString();

    SCH_SHEET_PATH sheet_path;
    GetSymbolSheetByKIID( aKID, sheet_path );

    aReference = aSymbol->GetRef( &sheet_path ).ToStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolKIIDByReference( const std::string& reference, std::vector<std::string>& aKIDList )
{
    aKIDList.clear();
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    wxString           w_reference = wxString::FromUTF8( reference );
    SCH_REFERENCE_LIST references;
    frame->Schematic().Hierarchy().GetSymbols( references , {});
    for( size_t i = 0; i < references.GetCount(); i++ )
    {
        SCH_SYMBOL* item = references[i].GetSymbol();
        // if (item->GetField( REFERENCE_FIELD )->GetText().CmpNoCase( w_reference ) == 0)
        if (item->GetField( FIELD_T::REFERENCE )->GetText().CmpNoCase( w_reference ) == 0)
        {
            aKIDList.push_back( item->m_Uuid.AsStdString() );
        }
        // if( item->GetField( REFERENCE_FIELD )->GetText().ToStdString() == reference )
        // {
        //     aKIDList.push_back( item->m_Uuid.AsStdString() );
        // }
    }
    // ;
    // for( auto& elem : frame->GetScreen()->Items().OfType( SCH_SYMBOL_T ) )
    // {
    //     SCH_SYMBOL* item = static_cast<SCH_SYMBOL*>( elem );

    //     if( item->GetField( REFERENCE_FIELD )->GetText().ToStdString() == reference )
    //         {
    //             aKIDList.push_back( item->m_Uuid.AsStdString() );
    //         }
    // }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolValueByKIID( const std::string& aKID, std::string& aValue )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // aValue = symbol->GetField( VALUE_FIELD )->GetText().ToStdString();
    aValue = symbol->GetField( FIELD_T::VALUE )->GetText().ToStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolFpByKIID( const std::string& aKID, std::string& aFootprint )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // aFootprint = symbol->GetField( FOOTPRINT_FIELD )->GetText().ToStdString();
    aFootprint = symbol->GetField( FIELD_T::FOOTPRINT )->GetText().ToStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolDataSheetByKIID( const std::string& aKID, std::string& aDataSet )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // aDataSet = symbol->GetField( DATASHEET_FIELD )->GetText().ToStdString();
    aDataSet = symbol->GetField( FIELD_T::DATASHEET )->GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolDescriptionByKIID( const std::string& aKID,
                                                                      std::string&       aDescription )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // aDescription = symbol->GetField( DESCRIPTION_FIELD )->GetText().ToStdString();
    aDescription = symbol->GetField( FIELD_T::DESCRIPTION )->GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::SetSymbolReferenceByKIID( const std::string& aKID,
                                                                    const std::string& aReference )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_SHEET_PATH sheet_path;
    GetSymbolSheetByKIID( aKID, sheet_path );

    // symbol->GetField( REFERENCE_FIELD )->SetText( aReference );
    wxString w_ref = wxString::FromUTF8( aReference );
    symbol->SetRef( &sheet_path, w_ref );

    frame->UpdateItem( symbol, false, true );

    frame->GetCanvas()->Refresh();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::SetSymbolValueByKIID( const std::string& aKID, const std::string& aValue )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // symbol->GetField( VALUE_FIELD )->SetText( aValue );
    symbol->GetField( FIELD_T::VALUE )->SetText( aValue );

    frame->UpdateItem( symbol, false, true );

    frame->GetCanvas()->Refresh();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::SetSymbolFpByKIID( const std::string& aKID, const std::string& aFootprint )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // symbol->GetField( FOOTPRINT_FIELD )->SetText( aFootprint );
    symbol->GetField( FIELD_T::FOOTPRINT )->SetText( aFootprint );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::SetSymbolDataSheetByKIID( const std::string& aKID,
                                                                    const std::string& aDataSheet )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // symbol->GetField( DATASHEET_FIELD )->SetText( aDataSheet );
    symbol->GetField( FIELD_T::DATASHEET )->SetText( aDataSheet );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::SetSymbolDescriptionByKIID( const std::string& aKID,
                                                                      const std::string& aDescription )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // symbol->GetField( DESCRIPTION_FIELD )->SetText( aDescription );
    symbol->GetField( FIELD_T::DESCRIPTION )->SetText( aDescription );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolLibName( const std::string& aKID, std::string& aLibNickNameName,
                                                            std::string& aLibItemName, std::string& aFormatName )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aLibNickNameName = symbol->GetLibId().GetLibNickname();
    aLibItemName = symbol->GetLibId().GetLibItemName();
    aFormatName = symbol->GetLibId().Format();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolFieldsMap( const std::string&                  aKID,
                                                              std::map<std::string, std::string>& aMap )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    for( auto& field : symbol->GetFields() )
    {
        std::string show_name = field.GetShownName().ToStdString();
        std::string show_text = field.GetShownText( true ).ToStdString();
        aMap.insert( std::pair<std::string, std::string>( show_name, show_text ) );
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::AddSymbolField( const std::string& aKID, const std::string& aName,
                                                          const std::string& aText, bool aVisible )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    // SCH_FIELD* field = new SCH_FIELD( symbol->GetPosition(), LAYER_FIELDS, symbol, aName );
    SCH_FIELD* field = new SCH_FIELD( symbol, FIELD_T::USER,  aName );
    field->SetText( aText );
    field->SetVisible( aVisible );
    symbol->AddField( *field );

    frame->UpdateItem( symbol );


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::RemoveSymbolField( const std::string& aKID, const std::string& aName )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    symbol->RemoveField( aName );

    frame->UpdateItem( symbol );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolPinCountsInAllUnits( const std::string& aKID, int& aFullNumber )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aFullNumber = symbol->GetFullPinCount();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}



KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolUnitPinNames( const std::string&        aKID,
                                                                 std::vector<std::string>& aNameList )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    for( auto& sch_pin : symbol->GetPins() )
    {
        aNameList.push_back( sch_pin->GetShownName().ToStdString() );
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolUnitPinNumberList( const std::string&        aKID,
                                                                      std::vector<std::string>& aNumberList )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    for( auto& sch_pin : symbol->GetPins() )
    {
        aNumberList.push_back( sch_pin->GetNumber().ToStdString() );
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolUnitPinNameByNumber( const std::string& aKID,
                                                                        const std::string& aNumber, std::string& aName )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }


    aName = sch_pin->GetShownName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolUnitPinPosition( const std::string aKID, const std::string& aNumber,
                                                                    VECTOR2I& aPosition )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPosition = sch_pin->GetPosition();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolBodyBoundBox( const std::string& aKID, BOX2I& aBox )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aBox = symbol->GetBodyBoundingBox();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolUnitPinOrientation( const std::string& aKID,
                                                                       const std::string& aNumber,
                                                                       PIN_ORIENTATION&   aOrientation )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }
    aOrientation = sch_pin->GetOrientation();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolPinType( const std::string& aKID, const std::string& aNumber,
                                                            ELECTRICAL_PINTYPE& aPinType )
{
    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPinType = sch_pin->GetType();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::PlaceLabelOnSymbolPin( const std::string& aKID, const std::string& aNumber,
                                                                 const std::string& aNetLabel )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );
    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    VECTOR2I pin_position, end_position;
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != GetSymbolUnitPinPosition( aKID, aNumber, pin_position ))
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }


    BOX2I symbol_body_box;
    symbol_body_box = symbol->GetBodyBoundingBox();

    VECTOR2I symbol_box_center = symbol_body_box.GetCenter();

    int underline = SchMMToIU( 3 + aNetLabel.length() );
    int label_angle = 0;

    if( pin_position.x <= symbol_body_box.GetLeft() && pin_position.y >= symbol_body_box.GetTop() )
    {
        end_position = pin_position + VECTOR2I( -underline, 0 );
    }
    else if( pin_position.x >= symbol_body_box.GetRight() && pin_position.y >= symbol_body_box.GetTop() )
    {
        end_position = pin_position + VECTOR2I( underline, 0 );
        label_angle = 180;
    }
    else if( pin_position.x >= symbol_body_box.GetLeft() && pin_position.y <= symbol_body_box.GetTop() )
    {
        end_position = pin_position + VECTOR2I( 0, -underline );
        label_angle = 90;
    }
    else if( pin_position.x >= symbol_body_box.GetLeft() && pin_position.y >= symbol_body_box.GetBottom() )
    {
        end_position = pin_position + VECTOR2I( 0, underline );
        label_angle = 270;
    }
    else
    {
        // return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }


    SCH_PRIMITIVES_LINE line_operation( frame );
    std::string         wire_uuid;
    line_operation.CreateLine( pin_position, end_position, LAYER_WIRE, 0, false, wire_uuid );

    SCH_PRIMITIVES_LABEL label_operation;
    std::string          net_label_uuid;
    label_operation.CreateCommonLabel( end_position, aNetLabel, net_label_uuid );
    label_operation.RotateLabel( net_label_uuid, label_angle );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetSymbolPinKIID( const std::string& aKID, const std::string& aNumber,
                                                            std::string& aPinKID )
{
    SCH_SYMBOL* sch_symbol = nullptr;
    FindSymbolByKIID( aKID, sch_symbol );

    if( nullptr == sch_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( sch_symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPinKID = sch_pin->m_Uuid.AsStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::MoveSymbolByKIID( const std::string& aKID, const VECTOR2I& aOffset )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_SYMBOL* sch_symbol = nullptr;
    FindSymbolByKIID( aKID, sch_symbol );

    if( nullptr == sch_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_symbol->Move( aOffset );
    frame->UpdateItem( sch_symbol , false, true);

    frame->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::RotateSymbolByKIID( const std::string& aKID, bool aCCW )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_SYMBOL* sch_symbol = nullptr;
    FindSymbolByKIID( aKID, sch_symbol );

    if( nullptr == sch_symbol )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    sch_symbol->Rotate( sch_symbol->GetBodyBoundingBox().GetCenter(), aCCW );
    frame->UpdateItem( sch_symbol , false, true);
    frame->GetCanvas()->Refresh();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::UnHighlightSymbolPinByNumber( const std::string& aKID, const std::string& aNumber , bool aMarker)
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );
    if( nullptr == symbol )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    SCH_SHEET_PATH sheet_path;
    GetSymbolSheetByKIID( aKID, sheet_path );

    if( sheet_path.empty() )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;


    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    sch_pin->ClearBrightened();

    frame->GetCanvas()->GetView()->Update( symbol, KIGFX::VIEW_UPDATE_FLAGS::REPAINT ); //sch_pin  symbol

    SCH_ITEM_VEC items_ist = sch_pin->ConnectedItems( frame->GetCurrentSheet() );
    for( auto& item : items_ist )
    {
        item->ClearBrightened();
        frame->GetCanvas()->GetView()->Update( item, KIGFX::VIEW_UPDATE_FLAGS::REPAINT );
    }

    if( aMarker )
    {
        std::vector<SCH_MARKER*> allMarkers;
        SCH_PRIMITIVES_MARKER    marker_operation;
        std::string              mark_uuid;
        marker_operation.ClearMarkerAtPosition( sch_pin->GetPosition(), sheet_path );
    }


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::HighlightSymbolPinByNumber( const std::string&    aKID,
                                                                      const std::string&    aNumber,
                                                                      const KIGFX::COLOR4D& aColor,
                                                                      bool                  aMarker )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_SYMBOL* symbol = nullptr;
    FindSymbolByKIID( aKID, symbol );
    if( nullptr == symbol )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    SCH_SHEET_PATH sheet_path;
    GetSymbolSheetByKIID( aKID, sheet_path );

    if( sheet_path.empty())
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    

    SCH_PIN* sch_pin = nullptr;
    FindSymbolPinByNumber( symbol, aNumber, sch_pin );

    if( nullptr == sch_pin )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;


    sch_pin->SetBrightened();

    // if( COLOR4D::UNSPECIFIED != aColor )
    //     sch_pin->SetPinColor( aColor );

    // if( nullptr != sch_pin->GetLibPin() && COLOR4D::UNSPECIFIED != aColor )
    //     sch_pin->GetLibPin()->SetPinColor( aColor );

    frame->GetCanvas()->GetView()->Update( symbol, KIGFX::VIEW_UPDATE_FLAGS::REPAINT ); //sch_pin  symbol

    SCH_ITEM_VEC items_ist = sch_pin->ConnectedItems( frame->GetCurrentSheet() );  
    for( auto& item : items_ist )
    {
        SCH_LINE* line = dynamic_cast<SCH_LINE*>( item );
        if( line && COLOR4D::UNSPECIFIED != aColor )
        {
            line->SetLineColor( aColor );
            frame->UpdateItem( line );
        }
        else
        {
            item->SetBrightened();
            frame->GetCanvas()->GetView()->Update( item, KIGFX::VIEW_UPDATE_FLAGS::REPAINT );
        }
        
    }

    if( aMarker )
    {
        std::vector<SCH_MARKER*> allMarkers;
        SCH_PRIMITIVES_MARKER marker_operation;
        std::string mark_uuid;
        marker_operation.AddMarkerAtPosition( sch_pin->GetPosition(), sheet_path, sch_pin, mark_uuid );

    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SYMBOL::GetAllSymbolLibLibrary( std::vector<std::string>& aLibraryName )
{
    aLibraryName.clear();
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    // SYMBOL_LIB_TABLE* libs = PROJECT_SCH::SchSymbolLibTable( &(frame->Schematic().Prj()) );
    // for( int i = 0; i < libs->GetCount(); i++)
    // {
    //     auto& row = libs->At( i );
    //     aLibraryName.push_back(row.GetNickName().ToStdString());
    // }
    // SYMBOL_LIB_TABLE* glibs = &SYMBOL_LIB_TABLE::GetGlobalLibTable();
    // for( int i = 0; i < glibs->GetCount(); i++)
    // {
    //     auto& row = glibs->At( i );
    //     aLibraryName.push_back( row.GetNickName().ToStdString() );
    // }

    LIBRARY_MANAGER&              manager = Pgm().GetLibraryManager();
    std::optional<LIBRARY_TABLE*> optTable = manager.Table( LIBRARY_TABLE_TYPE::SYMBOL, LIBRARY_TABLE_SCOPE::PROJECT );
    if( !optTable.has_value())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &frame->Prj() );

    std::vector<wxString> library_names = adapter->GetLibraryNames();
    for(const wxString& name : library_names)
    {
        aLibraryName.push_back(name.ToStdString());
    }


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}