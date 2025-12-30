#ifndef API_PRIMTIVES_PCB_DRAWER_H
#define API_PRIMTIVES_PCB_DRAWER_H

#include <cpp_sdk/api_primitives_drawer.h>

class PCB_BASE_EDIT_FRAME;
struct API_CIRCLE;
struct API_ARC;
struct API_RECTANGLE;
struct API_BEZIER;
struct API_TEXTBOX;
struct API_TABLE;
struct API_LABEL;
struct API_LINE;
struct API_PCB_TRACK;
struct API_PCB_VIA;
struct API_PCB_PAD;
struct API_MOVE_PCB_PAD;
struct API_ROTATE_PCB_PAD;
struct API_SET_PAD_NUMBER;
struct API_SET_PAD_SIZE;
struct API_SET_PAD_DRILL_SHAPE;
struct API_SET_PAD_POSITION;
struct API_MULTI_LINES;
struct API_PCB_LAYER_NAME_LIST;
struct API_PCB_FOOTPRINT_PARAMS_LIST;
struct API_PCB_REFERENCE_LIST;
struct API_MOVE_PCB_FOOTPRINT;
struct API_SYMBOL_REFERENCE;
struct API_SET_FOOTPRINT_POSITION;
struct API_ROTATE_FOOTPRINT;
struct API_FRAME_PARAMS;

class API_PRIMTIVES_PCB_DRAWER : public API_PRIMITIVES_DRAWER
{
public:
    explicit API_PRIMTIVES_PCB_DRAWER( PCB_BASE_EDIT_FRAME* aFrame );

public:
    void DrawMultiLines(API_MULTI_LINES const& aParams);

    void DrawCircle( API_CIRCLE const& aParams );

    void DrawArc( API_ARC const& aParams );

    void DrawRectangle( API_RECTANGLE const& aParams );

    void DrawBezier( API_BEZIER const& aParams);

    void DrawTextBox( API_TEXTBOX const& aParams );

    void DrawTable( API_TABLE const& aParams);

    void DrawLabel( API_LABEL const& aParams);

    void DrawSingleLine( API_LINE const& aParams );

    void DrawPcbTrack( API_PCB_TRACK const& aParams);

    void PlaceVia( API_PCB_VIA const& aParams );

    void CreatePcbPad( API_PCB_PAD const& aParams );

    void MovePcbPad( API_MOVE_PCB_PAD const& aParams );

    void RotatePcbPad( API_ROTATE_PCB_PAD const& aParams );

    void SetPcbPadNumber( API_SET_PAD_NUMBER const& aParams );

    void SetPcbPadSize( API_SET_PAD_SIZE const& aParams);

    void SetPcbPadDrillSize( API_SET_PAD_SIZE const& aParams );

    void SetPcbPadDrillType( API_SET_PAD_DRILL_SHAPE const& aParams);

    void SetPcbPadPosition(API_SET_PAD_POSITION const& aParams);

    void GetPcbLayerNamesList( API_PCB_LAYER_NAME_LIST& aParams);

    void GetAllPcbFootprintList( API_PCB_FOOTPRINT_PARAMS_LIST& aParams);

    void GetPcbFootprintList( API_PCB_REFERENCE_LIST const& aParams, API_PCB_FOOTPRINT_PARAMS_LIST& aOut);

    void MovePcbFootprint( API_MOVE_PCB_FOOTPRINT const& aParams );

    void ModifyFootprintReference( API_SYMBOL_REFERENCE const& aParams );

    void SetFootprintPosition( API_SET_FOOTPRINT_POSITION const& aParams );

    void RotateFootprint(API_ROTATE_FOOTPRINT const& aParams);

private:
    PCB_BASE_EDIT_FRAME* getBaseFrame();

};



#endif