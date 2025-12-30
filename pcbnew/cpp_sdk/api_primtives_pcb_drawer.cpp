#include "api_primitives_pcb_drawer.h"
#include "pcb_cpp_sdk.h"

#include <pcb_base_edit_frame.h>
#include <board_design_settings.h>
#include <pcb_track.h>
#include <kiway.h>
#include <cpp_sdk/api_primitives_drawer.h>


API_PRIMTIVES_PCB_DRAWER::API_PRIMTIVES_PCB_DRAWER( PCB_BASE_EDIT_FRAME* aFrame ):
                         API_PRIMITIVES_DRAWER( aFrame)
{
}


PCB_BASE_EDIT_FRAME* API_PRIMTIVES_PCB_DRAWER::getBaseFrame()
{
    return static_cast<PCB_BASE_EDIT_FRAME*>( m_frame );
}

void API_PRIMTIVES_PCB_DRAWER::DrawCircle( API_CIRCLE const& aParams )
{
    int      cr = PcbMMToIU( aParams.radius );
    VECTOR2I center = PcbMMToIU( aParams.center );

    PCB_PRIMITIVES_CIRCLE circle_operation( getBaseFrame() );
    std::string           uuid;
    circle_operation.CreateCircle( center, cr, uuid );
    return;
}

void API_PRIMTIVES_PCB_DRAWER::DrawArc( API_ARC const& aParams )
{
    VECTOR2I start_pos, end_pos, center_pos;
    start_pos = PcbMMToIU( aParams.start);
    end_pos = PcbMMToIU( aParams.end );
    center_pos = PcbMMToIU( aParams.center );
    PCB_PRIMITIVES_ARC arc_operation( getBaseFrame() );
    std::string        aKID; 
    arc_operation.CreateArc( center_pos, start_pos, end_pos, aKID );
    return;
}


void API_PRIMTIVES_PCB_DRAWER::DrawRectangle( API_RECTANGLE const& aParams)
{
    VECTOR2I position = PcbMMToIU( aParams.top_left );
    int      width = PcbMMToIU( aParams.width );
    int      height = PcbMMToIU( aParams.height );
    PCB_PRIMITIVES_RECTANGLE rectangle_operation( getBaseFrame() );
    std::string              aKID;
    rectangle_operation.CreateRectangle( position, width, height, aKID );
}


void API_PRIMTIVES_PCB_DRAWER::DrawBezier( API_BEZIER const& aParams)
{
    VECTOR2I start = PcbMMToIU( aParams.start );
    VECTOR2I c1 = PcbMMToIU( aParams.c1 );
    VECTOR2I c2 = PcbMMToIU( aParams.c2 );
    VECTOR2I end = PcbMMToIU( aParams.end );
    PCB_PRIMITIVES_BEZIER bezier_operation( getBaseFrame() );
    std::string           aKID;
    bezier_operation.CreateBezier( start, c1, c2, end, aKID );
}

void API_PRIMTIVES_PCB_DRAWER::DrawTextBox( API_TEXTBOX const& aParams )
{
    VECTOR2I               position = PcbMMToIU( aParams.box.top_left );
    int                    width = PcbMMToIU( aParams.box.width );
    int                    height = PcbMMToIU( aParams.box.height );
    PCB_PRIMITIVES_TEXTBOX textbox_operation( getBaseFrame() );
    std::string            aKID;
    textbox_operation.CreateTextBox( position, width, height, aParams.text, aKID);
}

void API_PRIMTIVES_PCB_DRAWER::DrawTable( API_TABLE const& aParams )
{
    VECTOR2I position = PcbMMToIU(aParams.pos);
    PCB_PRIMITIVES_TABLE table_operation( getBaseFrame() );
    std::string          aKID;
    VECTOR2I             gridSize = { 1000000, 1000000 };
    table_operation.CreateTable( position, gridSize, aParams.rows, aParams.cols, aKID );
    return;
}

void API_PRIMTIVES_PCB_DRAWER::DrawLabel( API_LABEL const& aParams )
{
    VECTOR2I position = PcbMMToIU( aParams.position );
    PCB_PRIMITIVES_TEXTBOX textbox_operation( getBaseFrame() );
    std::string          aKID;
    textbox_operation.CreateText( position, aParams.text, aKID );
    return;
}

void API_PRIMTIVES_PCB_DRAWER::DrawSingleLine( API_LINE const& aParams )
{
    VECTOR2I start = PcbMMToIU( aParams.start );
    VECTOR2I end = PcbMMToIU( aParams.end );
    PCB_PRIMITIVES_LINE line_operation( getBaseFrame() );
    PCB_LAYER_ID layer = getBaseFrame()->GetActiveLayer();
    std::string  aKID;
    line_operation.CreateLine( start, end, layer, 0, false, aKID );
    return;
}

void API_PRIMTIVES_PCB_DRAWER::DrawMultiLines( API_MULTI_LINES const& aParams )
{
    auto&               lines = aParams.lines;
    PCB_PRIMITIVES_LINE line_operation( getBaseFrame() );

    for( auto& line : lines )
    {
        VECTOR2I    start = PcbMMToIU( line.start );
        VECTOR2I    end = PcbMMToIU( line.end );
        std::string aKID;
        PCB_LAYER_ID layer = getBaseFrame()->GetActiveLayer();
        line_operation.CreateLine( start, end,  layer, 0, false, aKID );
    }
    return;
}


void API_PRIMTIVES_PCB_DRAWER::DrawPcbTrack( API_PCB_TRACK const& aParams)
{
    VECTOR2I             start = PcbMMToIU(aParams.start);
    VECTOR2I             end = PcbMMToIU( aParams.end );

    BOARD* board;
    GetPcbBoard( board );

    PCB_LAYER_ID layer_id = PCB_LAYER_ID::UNDEFINED_LAYER;
    if( !aParams.layer_name.pcb_layer_name.empty() )
    {
        layer_id = board->GetLayerID( aParams.layer_name.pcb_layer_name );
    }
    PCB_PRIMITIVES_TRACK track_operation;
    std::string          aKID;
    track_operation.CreateTrack( 0, start, end, layer_id, 0, false, aKID );
    return;
}


void API_PRIMTIVES_PCB_DRAWER::PlaceVia( API_PCB_VIA const& aParams )
{
    VECTOR2I           position = PcbMMToIU( aParams.position );
    PCB_PRIMITIVES_VIA via_operation;

    BOARD* board = nullptr;
    GetPcbBoard( board );
    BOARD_DESIGN_SETTINGS& bds = board->GetDesignSettings();

    int width = bds.GetCurrentViaSize();
    int drill = bds.GetCurrentViaDrill();

    std::string aKID;

    VIATYPE      viaType = VIATYPE::THROUGH; //VIATYPE::MICROVIA;//VIATYPE::THROUGH;
    PCB_LAYER_ID firstLayer = PCB_LAYER_ID::F_Cu;
    PCB_LAYER_ID lastLayer = PCB_LAYER_ID::B_Cu;

    via_operation.CreateVia( 0, position, viaType, firstLayer, lastLayer, drill, width, false, aKID );
    return;
}


void API_PRIMTIVES_PCB_DRAWER::CreatePcbPad( API_PCB_PAD const& aParams )
{
    VECTOR2I position = PcbMMToIU( aParams.position );

    PCB_PRIMITIVES_PAD pad_operation;
    std::string        aKID;
    pad_operation.PlacePad( aParams.number, position, aKID );
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}


void API_PRIMTIVES_PCB_DRAWER::MovePcbPad( API_MOVE_PCB_PAD const& aParams )
{
    VECTOR2I offset = PcbMMToIU( aParams.offset );
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    for( auto& pad_uuid : pad_list)
    {
        pad_operation.MovePad( pad_uuid, offset );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}


void API_PRIMTIVES_PCB_DRAWER::RotatePcbPad( API_ROTATE_PCB_PAD const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    for( auto& pad_uuid : pad_list)
    {
        pad_operation.RotatePad( pad_uuid, aParams.degree );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}

void API_PRIMTIVES_PCB_DRAWER::SetPcbPadNumber( API_SET_PAD_NUMBER const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.old_number, pad_list );

    for( auto& pad_uuid : pad_list)
    {
        pad_operation.SetPadNumber( pad_uuid, aParams.new_number );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}


void API_PRIMTIVES_PCB_DRAWER::SetPcbPadSize( API_SET_PAD_SIZE const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    VECTOR2I size = PcbMMToIU( aParams.size );
    for(auto& pad_uuid : pad_list)
    {
        pad_operation.SetPadStackSize( pad_uuid, size );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}

void API_PRIMTIVES_PCB_DRAWER::SetPcbPadDrillSize( API_SET_PAD_SIZE const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    VECTOR2I size = PcbMMToIU( aParams.size );

    for(auto& pad_uuid : pad_list)
    {
        pad_operation.SetPadStackDrillSize( pad_uuid, size );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;
}

void API_PRIMTIVES_PCB_DRAWER::SetPcbPadDrillType( API_SET_PAD_DRILL_SHAPE const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    std::string type = aParams.shape;

    auto shape = magic_enum::enum_cast<PAD_DRILL_SHAPE>( type );
    
    if(!shape.has_value())
    {
        return;
    }

    for(auto& pad_uuid : pad_list)
    {
        pad_operation.SetPadDrillShape( pad_uuid, *shape );
    }
    getBaseFrame()->GetCanvas()->Refresh();
    return;

}


void API_PRIMTIVES_PCB_DRAWER::SetPcbPadPosition( API_SET_PAD_POSITION const& aParams )
{
    std::vector<std::string> pad_list;
    PCB_PRIMITIVES_PAD       pad_operation;
    pad_operation.GetPadKIDByNumber( aParams.number, pad_list );

    VECTOR2I position = PcbMMToIU( aParams.new_position );
    for(auto& pad_uuid : pad_list)
    {
        pad_operation.SetPadPosition( pad_uuid, position );
    }
    getBaseFrame()->GetCanvas()->Refresh();
}

void API_PRIMTIVES_PCB_DRAWER::GetPcbLayerNamesList( API_PCB_LAYER_NAME_LIST& aParams )
{
    aParams.pcb_layer_name_list.clear();

    BOARD* board = nullptr;
    GetPcbBoard(board);

    if( nullptr == board ) return;

    for( int layer = F_Cu; layer < PCB_LAYER_ID_COUNT; ++layer )
    {
        wxString layer_name = board->GetLayerName( ToLAYER_ID( layer ) );
        aParams.pcb_layer_name_list.push_back( API_PCB_LAYER_NAME( layer_name.ToStdString() ) );
    }
    return;
}


void API_PRIMTIVES_PCB_DRAWER::GetAllPcbFootprintList( API_PCB_FOOTPRINT_PARAMS_LIST& aParams )
{
    aParams.footprint_list.clear();
    PCB_PRIMITIVES_FOOTPRINT fp_operation;
    std::vector<std::string> fp_uuid_list;

    fp_operation.GetAllFootprintKIID( fp_uuid_list );
    for( auto& fp_uuid : fp_uuid_list)
    {
        std::string fpid;
        fp_operation.GetFootprintFPID( fp_uuid, fpid );
        std::string reference;
        fp_operation.GetFootprintReference( fp_uuid, reference );
        VECTOR2I position;
        fp_operation.GetFootprintPosition( fp_uuid, position);

        aParams.footprint_list.emplace_back( reference, fpid, PcbIUToMM( position ) );
    }
    return;
}

void API_PRIMTIVES_PCB_DRAWER::GetPcbFootprintList( API_PCB_REFERENCE_LIST const&  aParams,
                                                    API_PCB_FOOTPRINT_PARAMS_LIST& aOut )
{
    PCB_PRIMITIVES_FOOTPRINT fp_operation;

    for( auto& fp : aParams.reference_list )
    {
        std::string fp_uuid;
        fp_operation.GetKIIDByReference( fp.reference, fp_uuid );
        std::string fpid;
        fp_operation.GetFootprintFPID( fp_uuid, fpid );

        VECTOR2I position;
        fp_operation.GetFootprintPosition( fp_uuid, position );

        aOut.footprint_list.emplace_back( fp.reference, fpid, PcbIUToMM( position ) );
    }
    return;
}

void API_PRIMTIVES_PCB_DRAWER::MovePcbFootprint( API_MOVE_PCB_FOOTPRINT const& aParams )
{
    PCB_PRIMITIVES_FOOTPRINT fp_operation;

    std::string fp_uuid;
    fp_operation.GetKIIDByReference( aParams.reference, fp_uuid );

    if( !fp_uuid.empty())
    {
        fp_operation.MoveFootprintByKIID( fp_uuid, PcbMMToIU( aParams.offset ) );
    }
}


void API_PRIMTIVES_PCB_DRAWER::ModifyFootprintReference( API_SYMBOL_REFERENCE const& aParams )
{
    PCB_PRIMITIVES_FOOTPRINT fp_operation;

    std::string fp_uuid;
    fp_operation.GetKIIDByReference( aParams.old_reference, fp_uuid );

    if( !fp_uuid.empty())
    {
        fp_operation.SetFootprintReference( fp_uuid, aParams.new_reference );
    }
}

void API_PRIMTIVES_PCB_DRAWER::SetFootprintPosition( API_SET_FOOTPRINT_POSITION const& aParams )
{
    PCB_PRIMITIVES_FOOTPRINT fp_operation;

    std::string              fp_uuid;
    fp_operation.GetKIIDByReference( aParams.reference, fp_uuid );
    
    if( !fp_uuid.empty())
    {
        fp_operation.SetFootprintPosition( fp_uuid, PcbMMToIU( aParams.new_position ) );
    }
}


void API_PRIMTIVES_PCB_DRAWER::RotateFootprint( API_ROTATE_FOOTPRINT const& aParams )
{
    PCB_PRIMITIVES_FOOTPRINT fp_operation;
    std::string              fp_uuid;
    fp_operation.GetKIIDByReference( aParams.reference, fp_uuid );

    if( !fp_uuid.empty())
    {
        fp_operation.RotateFootprintByKIID( fp_uuid, aParams.degree );
    }
    
}



