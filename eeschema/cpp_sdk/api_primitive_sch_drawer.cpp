#include <cpp_sdk/cpp_sdk_sch_api.h>
#include <cpp_sdk/api_cpp_sdk_param.h>
#include <sch_base_frame.h>
#include <sch_symbol.h>
#include <sch_pin.h>
#include <sch_sheet_path.h>
#include <gal/color4d.h>
#include "api_primitive_sch_drawer.h"
#include "netlist_exporter_base.h"
#include <kiway.h>
#include <eeschema_id.h>
#include <tools/sch_actions.h>

API_PRIMITIVE_SCH_DRAWER::API_PRIMITIVE_SCH_DRAWER( SCH_BASE_FRAME* aFrame ):
                            API_PRIMITIVES_DRAWER( aFrame )
{
}


SCH_BASE_FRAME* API_PRIMITIVE_SCH_DRAWER::getBaseFrame()
{
    return static_cast<SCH_BASE_FRAME*>(m_frame);
}


void API_PRIMITIVE_SCH_DRAWER::DrawWire( API_LINE const& aParams )
{
    VECTOR2I            start = SchMMToIU(aParams.start);
    VECTOR2I            end = SchMMToIU(aParams.end);
    SCH_PRIMITIVES_LINE wire_operation( getBaseFrame() );
    std::string         wire_uuid;

    auto layer_id = SCH_LAYER_ID::LAYER_WIRE;

    if( GetFrameType()== FRAME_T::FRAME_SCH_SYMBOL_EDITOR )
    {
        layer_id = SCH_LAYER_ID::LAYER_NOTES;
    }
    wire_operation.CreateLine( start, end, layer_id, 0, false, wire_uuid );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawMultiLines( API_MULTI_LINES const& aParams )
{
    auto&               lines = aParams.lines;
    SCH_PRIMITIVES_LINE line_operation( getBaseFrame() );

    for( auto& line : lines )
    {
        VECTOR2I    start = SchMMToIU( line.start );
        VECTOR2I    end = SchMMToIU( line.end );
        std::string aKID;
        line_operation.CreateLine( start, end, SCH_LAYER_ID::LAYER_NOTES, 0, false, aKID );
    }

    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawMultiWires( API_MULTI_LINES const& aParams )
{
    auto& lines = aParams.lines;
    SCH_PRIMITIVES_LINE wire_operation( getBaseFrame() );

    for( auto& line : lines )
    {
        VECTOR2I    start = SchMMToIU( line.start );
        VECTOR2I    end = SchMMToIU( line.end );
        std::string aKID;
        wire_operation.CreateLine( start, end, SCH_LAYER_ID::LAYER_WIRE, 0, false, aKID );
    }
 
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawBus( API_MULTI_LINES const& aParams )
{
    auto& buses = aParams.lines;
    SCH_PRIMITIVES_LINE bus_operation( getBaseFrame() );

    for( auto& line : buses )
    {
        VECTOR2I start = SchMMToIU( line.start );
        VECTOR2I end = SchMMToIU( line.end );
        std::string aKID;
        bus_operation.CreateLine( start, end, SCH_LAYER_ID::LAYER_BUS, 0, false, aKID );
    }

    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawCircle( API_CIRCLE const& aParams )
{
    double cx = SchMMToIU(aParams.center.x);
    double cy = SchMMToIU( aParams.center.y );
    double   cr = SchMMToIU( aParams.radius );
    VECTOR2I center( cx, cy );

    SCH_PRIMITIVES_CIRCLE circle_operation( getBaseFrame() );
    std::string uuid;
    circle_operation.CreateCircle( center, cr, uuid );
}

void API_PRIMITIVE_SCH_DRAWER::DrawArc( API_ARC const& aParams )
{
    VECTOR2I start_pos, end_pos;
    start_pos.x = SchMMToIU( aParams.start.x );
    start_pos.y = SchMMToIU( aParams.start.y );

    end_pos.x = SchMMToIU( aParams.end.x );
    end_pos.y = SchMMToIU( aParams.end.y );

    SCH_PRIMITIVES_ARC arc_operation( getBaseFrame() );
    std::string        aKID;
    arc_operation.CreateArc( start_pos, end_pos, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawBezier( API_BEZIER const& aParams )
{
    VECTOR2I    start = SchMMToIU( aParams.start );
    VECTOR2I    c1 = SchMMToIU( aParams.c1 );
    VECTOR2I    c2 = SchMMToIU( aParams.c2 );
    VECTOR2I    end = SchMMToIU( aParams.end );

    SCH_PRIMITIVES_BEZIER bezier_operation( getBaseFrame() );
    std::string           aKID;
    bezier_operation.CreateBezier( start, c1, c2, end, aKID );
}

void API_PRIMITIVE_SCH_DRAWER::DrawTextbox( API_TEXTBOX const& aParams )
{
    VECTOR2I top_left = SchMMToIU( aParams.box.top_left );
    int      width = SchMMToIU( aParams.box.width );
    int                    height = SchMMToIU( aParams.box.height );
    SCH_PRIMITIVES_TEXTBOX textbox_operation( getBaseFrame() );
    std::string            aKID;
    textbox_operation.CreateTextBox( top_left, width, height, aParams.text, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawTable( API_TABLE const& aParams )
{
    int                  rows = aParams.rows;
    int                  cols = aParams.cols;
    VECTOR2I             position = SchMMToIU( aParams.pos );
    SCH_PRIMITIVES_TABLE table_operation;
    std::string          aKID;
    table_operation.CreateTable( position, rows, cols, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawRectangle( API_RECTANGLE const& aParams )
{
    VECTOR2I       pos;
    pos.x = SchMMToIU( aParams.top_left.x );
    pos.y = SchMMToIU( aParams.top_left.y );
    int                      width = SchMMToIU( aParams.width );
    int                      height = SchMMToIU( aParams.height );
    SCH_PRIMITIVES_RECTANGLE rectangle_operation( getBaseFrame() );
    std::string              aKID;
    rectangle_operation.CreateRectangle( pos, width, height, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawSchematicText( API_LABEL const& aParams )
{
    VECTOR2I                      position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_SCHEMATIC_TEXT sch_text_opeartion( getBaseFrame() );
    std::string                   aKID;
    sch_text_opeartion.CreateSchematicText( position, aParams.text, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::DrawHierSheet( API_HIER_SHEET const& aParams )
{
    VECTOR2I             position = SchMMToIU( aParams.box.top_left );
    int                  width = SchMMToIU( aParams.box.width );
    int                  height = SchMMToIU( aParams.box.height );
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          aKID;
    sheet_operation.CreateHierSheet( position, width, height, aParams.title, aKID );
    return; 
}

void API_PRIMITIVE_SCH_DRAWER::PlaceLocalLabel( API_LABEL const& aParams )
{
    VECTOR2I             position = SchMMToIU( aParams.position );
    std::string          aKID;
    SCH_PRIMITIVES_LABEL label_operation;
    label_operation.CreateCommonLabel( position, aParams.text, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::PlaceGlobalLabel( API_LABEL const& aParams )
{
    VECTOR2I   position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_LABEL label_operation;
    std::string          aKID;
    label_operation.CreateGlobalLabel( position, aParams.text, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::PlaceHierLabel( API_LABEL const& aParams )
{
    VECTOR2I             position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_LABEL label_operation;
    std::string          aKID;
    label_operation.CreateHierLabel( position, aParams.text, aKID );
    return;
}

void API_PRIMITIVE_SCH_DRAWER::PlaceClassLabel( API_CLASS_LABEL const& aParams )
{
    VECTOR2I position = SchMMToIU( aParams.position );

    SCH_PRIMITIVES_LABEL label_operation;
    std::string          aKID;
    label_operation.CreateClassLabel( position, aParams.net_class, aParams.component_class, aKID );
}

void API_PRIMITIVE_SCH_DRAWER::PlaceSymbolPinLabel( API_SYMBOL_PIN_LABEL const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;

    std::vector<std::string> symbol_uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.reference, symbol_uuid_list );

    if( symbol_uuid_list.empty() )
        return;

    for(auto& symbol_uuid : symbol_uuid_list)
    {
        symbol_operation.PlaceLabelOnSymbolPin( symbol_uuid, aParams.pin_number, aParams.label_text );
    }
    

}

void API_PRIMITIVE_SCH_DRAWER::GetSymbolLibrary( API_SYMBOL_LIBRARY_LIST& aLibraryList )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::vector<std::string> list;
    symbol_operation.GetAllSymbolLibLibrary( list );
    for( auto& name : list)
    {
        aLibraryList.library_list.emplace_back( API_SYMBOL_LIBRARY{name} );
    }
}

void API_PRIMITIVE_SCH_DRAWER::PlaceSymbol( API_PLACE_SYMBOL const& aParams )
{
    VECTOR2I            position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string         symbol_uuid;

    std::string symbol_path;
    std::string symbol_lib_nick_name;

    auto category = magic_enum::enum_cast<API_SYMBOL_CATEGORY>( aParams.category );

    std::string symbol_lib_path;
    GetGlobalSymbolDir( symbol_lib_path );

    

    if( !category.has_value() )
    {
        return;
    }

    switch( *category )
    {
        case API_SYMBOL_CATEGORY::RESISTOR:
        {
            symbol_lib_nick_name = "R";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::CAPACITOR:
        {
            symbol_lib_nick_name = "C";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::INDUCTOR:
        {
            symbol_lib_nick_name = "L";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::POTENTIOMETER:
        {
            symbol_lib_nick_name = "R_Potentiometer";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::CRYSTAL_OSCILLATOR:
        {
            symbol_lib_nick_name = "Crystal";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::TRANSFORMER:
        {
            symbol_lib_nick_name = "TRANSF1";
            symbol_path = "Transformer.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::FUSE:
        {
            symbol_lib_nick_name = "Fuse";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::FERRITE_BEAD:
        {
            symbol_lib_nick_name = "FerriteBead";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::DIODE:
        {
            symbol_lib_nick_name = "D";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::ZENER_DIODE:
        {
            symbol_lib_nick_name = "D_Zener";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::SCHOTTKY_DIODE:
        {
            symbol_lib_nick_name = "D_Schottky";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::VARACTOR_DIODE:
        {
            symbol_lib_nick_name = "D_Capacitance";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::LED:
        {
            symbol_lib_nick_name = "LED";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::PHOTODIODE:
        {
            symbol_lib_nick_name = "D_Photo";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::TRANSISTOR_NPN:
        {
            symbol_lib_nick_name = "Q_NPN";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::TRANSISTOR_PNP:
        {
            symbol_lib_nick_name = "Q_PNP";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::MOSFET_N_CHANNEL:
        {
            symbol_lib_nick_name = "Q_NMOS";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::MOSFET_P_CHANNEL:
        {
            symbol_lib_nick_name = "Q_PMOS";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::JFET_N_CHANNEL:
        {
            symbol_lib_nick_name = "Q_NJFET_DGS";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::JFET_P_CHANNEL:
        {
            symbol_lib_nick_name = "Q_PJFET_DGS";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::IGBT:
        {
            symbol_lib_nick_name = "Q_NIGBT_CEG";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::THYRISTOR:
        {
            symbol_lib_nick_name = "Q_SCR_AGK";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::TRIAC:
        {
            symbol_lib_nick_name = "Q_Triac";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::OPERATIONAL_AMPLIFIER:
        {
            symbol_lib_nick_name = "Opamp_Dual";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::POWER_DC:
        {
            symbol_lib_nick_name = "VCC";
            symbol_path = "power.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::POWER_AC:
        {
            symbol_lib_nick_name = "AC";
            symbol_path = "power.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::GROUND:
        {
            symbol_lib_nick_name = "GND";
            symbol_path = "power.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::BUTTON:
        {
            symbol_lib_nick_name = "SW_Push";
            symbol_path = "Switch.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::BUZZER:
        {
            symbol_lib_nick_name = "Buzzer";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::SPEAKER:
        {
            symbol_lib_nick_name = "Speaker";
            symbol_path = "Device.kicad_sym";
            break;
        }
        case API_SYMBOL_CATEGORY::MOTOR:
        {
            symbol_lib_nick_name = "Motor_AC";
            symbol_path = "Motor.kicad_sym";
            break;
        }

        default:
            break;
    }


    if( symbol_path.empty() || symbol_lib_nick_name.empty())
    {
        return;
    }


    std::string symbol_full_path = symbol_lib_path + symbol_path;
    symbol_operation.PlaceSymbol( symbol_full_path, symbol_lib_nick_name, position, symbol_uuid );

    if( !symbol_uuid.empty() )
    {
        if( !aParams.value.empty() )
            symbol_operation.SetSymbolValueByKIID( symbol_uuid, aParams.value );

        if( !aParams.reference.empty() )
            symbol_operation.SetSymbolReferenceByKIID( symbol_uuid, aParams.reference );
    }

    return;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSymbolReference( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string           reference;
    symbol_operation.GetSymbolReferenceByKIID( aParams.kiid, reference );
    return reference;
}

void API_PRIMITIVE_SCH_DRAWER::SetSymbolReference( API_SYMBOL_REFERENCE const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;

    std::vector<std::string>   uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.old_reference, uuid_list);

    for( auto& uuid : uuid_list)
    {
        symbol_operation.SetSymbolReferenceByKIID( uuid, aParams.new_reference );
    }
    
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSymbolValue( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string           value;
    symbol_operation.GetSymbolValueByKIID( aParams.kiid, value );
    return value;
}

void API_PRIMITIVE_SCH_DRAWER::SetSymbolValue( API_SYMBOL_VALUE const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;

    std::vector<std::string> uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.reference, uuid_list );

    for( auto& uuid : uuid_list )
    {
        symbol_operation.SetSymbolValueByKIID( uuid, aParams.value );
    }
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSymbolFootprint( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string           footprint;
    symbol_operation.GetSymbolFpByKIID( aParams.kiid, footprint );
    return footprint;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSymbolDataSheet( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string           datasheet;
    symbol_operation.GetSymbolDataSheetByKIID( aParams.kiid, datasheet );
    return datasheet;
}

void API_PRIMITIVE_SCH_DRAWER::SetSymbolDataSheet( API_SYMBOL_DATASHEET const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;


    std::vector<std::string> uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.reference, uuid_list );

    for( auto& uuid : uuid_list )
    {
        symbol_operation.SetSymbolDataSheetByKIID( uuid, aParams.datasheet );
    }

    
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSymbolDescription( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::string           description;
    symbol_operation.GetSymbolDescriptionByKIID( aParams.kiid, description );
    return description;
}

void API_PRIMITIVE_SCH_DRAWER::SetSymbolDescription( API_SYMBOL_DESCRIPTION const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    symbol_operation.SetSymbolDescriptionByKIID( aParams.kiid, aParams.description );
}

std::map<std::string, std::string> API_PRIMITIVE_SCH_DRAWER::GetSymbolFields( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::map<std::string, std::string> fields_map;
    symbol_operation.GetSymbolFieldsMap( aParams.kiid, fields_map );
    return fields_map;
}

void API_PRIMITIVE_SCH_DRAWER::AddSymbolField( API_SYMBOL_FIELD const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    symbol_operation.AddSymbolField( aParams.kiid, aParams.name, aParams.text, aParams.visible );
}

void API_PRIMITIVE_SCH_DRAWER::RemoveSymbolField( API_SYMBOL_FIELD const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    symbol_operation.RemoveSymbolField( aParams.kiid, aParams.name );
}

std::vector<API_PIN_INFO> API_PRIMITIVE_SCH_DRAWER::GetSymbolPins( API_SYMBOL_KIID const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::vector<std::string> pin_numbers;
    symbol_operation.GetSymbolUnitPinNumberList( aParams.kiid, pin_numbers );
    
    std::vector<API_PIN_INFO> pins_info;
    for( const auto& pin_number : pin_numbers )
    {
        VECTOR2I pos;
        symbol_operation.GetSymbolUnitPinPosition( aParams.kiid, pin_number, pos );
        
        API_PIN_INFO pin_info;
        pin_info.pin_number = pin_number;
        pin_info.pin_position = { static_cast<double>( SchIUToMM( pos.x ) ), static_cast<double>( SchIUToMM( pos.y ) ) };
        pins_info.push_back( pin_info );
    }
    
    return pins_info;
}

API_POINT API_PRIMITIVE_SCH_DRAWER::GetSymbolPinPosition( API_SYMBOL_PIN_INFO const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    VECTOR2I              position;
    symbol_operation.GetSymbolUnitPinPosition( aParams.kiid, aParams.pin_number, position );
    return { static_cast<double>( SchIUToMM( position.x ) ), static_cast<double>( SchIUToMM( position.y ) ) };
}

int API_PRIMITIVE_SCH_DRAWER::GetSymbolPinOrientation( API_SYMBOL_PIN_INFO const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    PIN_ORIENTATION       orientation;
    symbol_operation.GetSymbolUnitPinOrientation( aParams.kiid, aParams.pin_number, orientation );
    return static_cast<int>( orientation );
}

int API_PRIMITIVE_SCH_DRAWER::GetSymbolPinType( API_SYMBOL_PIN_INFO const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    ELECTRICAL_PINTYPE    pin_type;
    symbol_operation.GetSymbolPinType( aParams.kiid, aParams.pin_number, pin_type );
    return static_cast<int>( pin_type );
}

void API_PRIMITIVE_SCH_DRAWER::HighlightSymbolPin( API_SYMBOL_PIN_HIGHLIGHT const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    // Convert color string to COLOR4D if needed, using default for now
    symbol_operation.HighlightSymbolPinByNumber( aParams.kiid, aParams.pin_number, KIGFX::COLOR4D::UNSPECIFIED, aParams.marker );
}

void API_PRIMITIVE_SCH_DRAWER::UnhighlightSymbolPin( API_SYMBOL_PIN_INFO const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    symbol_operation.UnHighlightSymbolPinByNumber( aParams.kiid, aParams.pin_number, true );
}

void API_PRIMITIVE_SCH_DRAWER::MoveSymbol( API_MOVE_SYMBOL const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    VECTOR2I              offset = SchMMToIU( aParams.offset );

    std::vector<std::string> uuid_list;
    symbol_operation.GetSymbolKIIDByReference(aParams.reference, uuid_list);

    if( !uuid_list.empty())
    {
        symbol_operation.MoveSymbolByKIID( uuid_list.front(), offset );
    }
    
}

void API_PRIMITIVE_SCH_DRAWER::RotateSymbol( API_ROTATE_SYMBOL const& aParams )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;

    std::vector<std::string> uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.reference, uuid_list );

    if( !uuid_list.empty())
    {
        symbol_operation.RotateSymbolByKIID( uuid_list.front(), aParams.ccw );
    }
    
}

// Sheet API Implementation
void API_PRIMITIVE_SCH_DRAWER::CreateSheetPin( API_CREATE_SHEET_PIN const& aParams )
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          kiid;
    sheet_operation.CreateSheetPin( aParams.pin_name, kiid );
}

std::string API_PRIMITIVE_SCH_DRAWER::GetRootSheet()
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          kiid;
    sheet_operation.GetRootSheetKID( kiid );
    return kiid;
}

std::vector<std::string> API_PRIMITIVE_SCH_DRAWER::GetSheetPathList()
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::vector<std::string> path_list;
    sheet_operation.GetRootSheetPathList( path_list );
    return path_list;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSheetPageNumber( API_GET_SHEET_PAGE_NUMBER const& aParams )
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          page_number;
    sheet_operation.GetSheetPageNumberByPath( aParams.path, page_number );
    return page_number;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetSheetByPageNumber( API_GET_SHEET_BY_PAGE_NUMBER const& aParams )
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          kiid;
    sheet_operation.GetSheetKIDByPageNumber( aParams.page_number, kiid );
    return kiid;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetCurrentSheet()
{
    SCH_PRIMITIVES_SHEET sheet_operation;
    std::string          kiid;
    sheet_operation.GetCurrentSheetLastScreen( kiid );
    return kiid;
}

// Marker API Implementation
void API_PRIMITIVE_SCH_DRAWER::AddMarker( API_ADD_MARKER const& aParams )
{
    SCH_PRIMITIVES_MARKER marker_operation;
    VECTOR2I              position = SchMMToIU( aParams.position );
    std::string           kiid;
    // Note: Create a temporary empty sheet path since we don't have access to the real one
    SCH_SHEET_PATH sheet_path;
    marker_operation.AddMarkerAtPosition( position, sheet_path, nullptr, kiid );
}

void API_PRIMITIVE_SCH_DRAWER::ClearMarker( API_CLEAR_MARKER const& aParams )
{
    SCH_PRIMITIVES_MARKER marker_operation;
    VECTOR2I              position = SchMMToIU( aParams.position );
    // Note: Create a temporary empty sheet path since we don't have access to the real one
    SCH_SHEET_PATH sheet_path;
    marker_operation.ClearMarkerAtPosition( position, sheet_path );
}

std::vector<std::string> API_PRIMITIVE_SCH_DRAWER::GetAllMarkers()
{
    SCH_PRIMITIVES_MARKER marker_operation;
    std::vector<std::string> kiid_list;
    marker_operation.GetAllMarkerKIID( kiid_list );
    return kiid_list;
}

std::string API_PRIMITIVE_SCH_DRAWER::FindMarkerByPosition( API_FIND_MARKER_BY_POSITION const& aParams )
{
    SCH_PRIMITIVES_MARKER marker_operation;
    VECTOR2I              position = SchMMToIU( aParams.position );
    std::string           kiid;
    marker_operation.FindMarkerKIDByPosition( position, kiid );
    return kiid;
}

// Line API implementations
void API_PRIMITIVE_SCH_DRAWER::CreateBusWireEntry( API_CREATE_BUS_WIRE_ENTRY const& aParams )
{
    VECTOR2I position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    std::string kiid;
    lineOperation.CreateBusWireEntry( position, kiid );
}

void API_PRIMITIVE_SCH_DRAWER::CreateJunction( API_CREATE_JUNCTION const& aParams )
{
    VECTOR2I position = SchMMToIU( aParams.position );
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    std::string kiid;
    lineOperation.CreateJunction( position, kiid );
}

std::vector<std::string> API_PRIMITIVE_SCH_DRAWER::GetAllLinesKIID( API_GET_ALL_LINES_KIID const& aParams )
{
    SCH_PRIMITIVES_LINE      lineOperation( getBaseFrame() );
    std::vector<std::string> allKIIDList;
    lineOperation.GetAllLinesKIID( static_cast<SCH_LAYER_ID>( aParams.layer ), allKIIDList );
    return allKIIDList;
}

double API_PRIMITIVE_SCH_DRAWER::GetLineWidth( API_LINE_KIID const& aParams )
{
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    double lineWidth = 0.0;
    lineOperation.GetLineWidth( aParams.kiid, lineWidth );
    return lineWidth;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetWireNetName( API_LINE_KIID const& aParams )
{
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    std::string netName;
    lineOperation.GetWireNetName( aParams.kiid, netName );
    return netName;
}

std::string API_PRIMITIVE_SCH_DRAWER::GetWireDriver( API_LINE_KIID const& aParams )
{
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    std::string driverName;
    lineOperation.GetWireDriver( aParams.kiid, driverName );
    return driverName;
}

void API_PRIMITIVE_SCH_DRAWER::SetLineColor( API_LINE_COLOR const& aParams )
{
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    // Convert RGB values to COLOR4D
    KIGFX::COLOR4D color( static_cast<double>( aParams.r ) / 255.0,
                          static_cast<double>( aParams.g ) / 255.0,
                          static_cast<double>( aParams.b ) / 255.0,
                          static_cast<double>( aParams.a ) / 255.0 );
    lineOperation.SetLineColor( aParams.kiid, color );
}

void API_PRIMITIVE_SCH_DRAWER::SetLineWidth( API_LINE_WIDTH const& aParams )
{
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    lineOperation.SetLineWidth( aParams.kiid, aParams.width );
}

void API_PRIMITIVE_SCH_DRAWER::MoveLine( API_MOVE_LINE const& aParams )
{
    VECTOR2I offset = SchMMToIU( aParams.offset );
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    lineOperation.MoveLine( aParams.kiid, offset, aParams.type );
}

void API_PRIMITIVE_SCH_DRAWER::SetLineStart( API_SET_LINE_START const& aParams )
{
    VECTOR2I start = SchMMToIU( aParams.start );
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    lineOperation.SetLineStart( aParams.kiid, start );
}

void API_PRIMITIVE_SCH_DRAWER::SetLineEnd( API_SET_LINE_END const& aParams )
{
    VECTOR2I end = SchMMToIU( aParams.end );
    SCH_PRIMITIVES_LINE lineOperation( getBaseFrame() );
    lineOperation.SetLineEnd( aParams.kiid, end );
}

void API_PRIMITIVE_SCH_DRAWER::GetSymbolPinsInfo( API_QUERY_SYMBOL const& aParams, API_SYMBOL_PINS_INFO& aSymbolPins )
{
    SCH_PRIMITIVES_SYMBOL symbol_operation;
    std::vector<std::string> symbol_uuid_list;
    symbol_operation.GetSymbolKIIDByReference( aParams.reference, symbol_uuid_list );
    aSymbolPins.reference = aParams.reference;

    for( auto& symbol_uuid : symbol_uuid_list )
    {
        if( aSymbolPins.symbol_name.empty() )
        {
            std::string libnick_name, item_name, format_name;
            symbol_operation.GetSymbolLibName( symbol_uuid, libnick_name, item_name, format_name );
            aSymbolPins.symbol_name = format_name;
        }
            
        std::vector<std::string> pin_number_list;
        symbol_operation.GetSymbolUnitPinNumberList( symbol_uuid, pin_number_list );
        for(auto& pin_number : pin_number_list)
        {
            VECTOR2I position;
            symbol_operation.GetSymbolUnitPinPosition( symbol_uuid, pin_number, position );
            std::string pin_name;
            symbol_operation.GetSymbolUnitPinNameByNumber( symbol_uuid, pin_number, pin_name );


            aSymbolPins.pins_info.emplace_back(API_PIN_INFO{
                    pin_number, pin_name,
                    SchIUToMM(position) }); //API_POINT{ (double) position.x, (double) position.y }
        }
    }
}

void API_PRIMITIVE_SCH_DRAWER::CreateSymbolLibrary( API_CREATE_SYMBOL_LIBRARY const& aParams )
{
    SCH_PRIMTIVES_LIB_SYMBOL lib_symbol_operation;
    lib_symbol_operation.CreateLibSymbolLibrary( aParams.symbol_library_name );

    lib_symbol_operation.CreateEmptyLibSymbol( aParams.symbol_library_name, "Symbol" );

    return;
}

void API_PRIMITIVE_SCH_DRAWER::CreateLibSymbolPin( API_LIB_SYMBOL_PIN const& aParams )
{
    VECTOR2I position = SchMMToIU( aParams.position );


    
    SCH_PRIMTIVES_LIB_SYMBOL lib_symbol_operation;
    lib_symbol_operation.CreateLibSymbolPin( aParams.pin_name, aParams.pin_number, position, PIN_ORIENTATION::PIN_LEFT,
                                             ELECTRICAL_PINTYPE::PT_OUTPUT, GRAPHIC_PINSHAPE::LINE, 120, 60, 60 );
}


void API_PRIMITIVE_SCH_DRAWER::ImportNonKicadSchematic()
{
    wxCommandEvent menuEvent( wxEVT_MENU, ID_IMPORT_NON_KICAD_SCH );

    SCH_EDIT_FRAME* frame = dynamic_cast<SCH_EDIT_FRAME*>( m_frame );
    if( frame )
    {
        frame->GetEventHandler()->ProcessEvent( menuEvent );
    }
}


void API_PRIMITIVE_SCH_DRAWER::ImportVectorGraphicFile()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::importGraphics );
}

void API_PRIMITIVE_SCH_DRAWER::ExportNetlist()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::exportNetlist );
}

void API_PRIMITIVE_SCH_DRAWER::OpenSchematicSetupDlg()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::schematicSetup );
}


void API_PRIMITIVE_SCH_DRAWER::OpenEditTextAndGraphicPropertyDlg()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::editTextAndGraphics );
}


void API_PRIMITIVE_SCH_DRAWER::ToggleHierarchyPanel()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::showHierarchy );
}

void API_PRIMITIVE_SCH_DRAWER::ToggleNetNavigatorPanel()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::showNetNavigator );
}

void API_PRIMITIVE_SCH_DRAWER::ToggleDesignBlockPanel()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::showDesignBlockPanel );
}

void API_PRIMITIVE_SCH_DRAWER::OpenSymbolLibraryBrowser()
{
    m_frame->GetToolManager()->RunAction( ACTIONS::showSymbolBrowser );
}

void API_PRIMITIVE_SCH_DRAWER::ShowBusSynatxHelp()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::showBusSyntaxHelp );
}

void API_PRIMITIVE_SCH_DRAWER::RunERC()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::runERC );
}

void API_PRIMITIVE_SCH_DRAWER::ShowSimulator()
{
    m_frame->GetToolManager()->RunAction( SCH_ACTIONS::showSimulator );
}