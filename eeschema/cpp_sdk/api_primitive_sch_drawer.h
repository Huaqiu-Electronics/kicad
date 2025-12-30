#ifndef API_PRIMITIVE_SCH_DRAWER_H
#define API_PRIMITIVE_SCH_DRAWER_H

#include <cpp_sdk/api_primitives_drawer.h>

class SCH_BASE_FRAME;
struct API_LINE;
struct API_MULTI_LINES;
struct API_MULTI_LINES;
struct API_CIRCLE;
struct API_ARC;
struct API_BEZIER;

class API_PRIMITIVE_SCH_DRAWER : public API_PRIMITIVES_DRAWER
{
public:
    explicit API_PRIMITIVE_SCH_DRAWER( SCH_BASE_FRAME* aFrame );

    void DrawMultiLines( API_MULTI_LINES const& aParams);

    void DrawWire( API_LINE const& aParams );

    void DrawMultiWires( API_MULTI_LINES const& aParams);

    void DrawBus( API_MULTI_LINES const& aParams );

    void DrawCircle( API_CIRCLE const& aParams );

    void DrawArc( API_ARC const& aParams );

    void DrawBezier( API_BEZIER const& aParams );

    void DrawTextbox( API_TEXTBOX const& aParams );

    void DrawTable( API_TABLE const& aParams);

    void DrawRectangle( API_RECTANGLE const& aParams );

    void DrawSchematicText( API_LABEL const& aParams );

    void DrawHierSheet( API_HIER_SHEET const& aParams);

    void PlaceLocalLabel( API_LABEL const& aParams);

    void PlaceGlobalLabel( API_LABEL const& aParams );

    void PlaceHierLabel( API_LABEL const& aParams );

    void PlaceClassLabel( API_CLASS_LABEL const& aParams );

    void PlaceSymbolPinLabel( API_SYMBOL_PIN_LABEL const& aParams );

    void GetSymbolPinsInfo( API_QUERY_SYMBOL const& aParams , API_SYMBOL_PINS_INFO& aSymbolPins );

    void GetSymbolLibrary( API_SYMBOL_LIBRARY_LIST& aLibraryList);

    // Symbol API
    void PlaceSymbol( API_PLACE_SYMBOL const& aParams );

    void MoveSymbol( API_MOVE_SYMBOL const& aParams );

    void CreateSymbolLibrary( API_CREATE_SYMBOL_LIBRARY const& aParams);

    void CreateLibSymbolPin( API_LIB_SYMBOL_PIN const& aParams);
    
    //
    void ImportNonKicadSchematic();

    void ImportVectorGraphicFile();

    void ExportNetlist();

    void OpenSchematicSetupDlg();

    void OpenEditTextAndGraphicPropertyDlg();

    void ToggleHierarchyPanel();

    void ToggleNetNavigatorPanel();

    void ToggleDesignBlockPanel();

    void OpenSymbolLibraryBrowser();

    void ShowBusSynatxHelp();

    void RunERC();

    void ShowSimulator();


    std::string                        GetSymbolReference( API_SYMBOL_KIID const& aParams );
    void SetSymbolReference( API_SYMBOL_REFERENCE const& aParams );
    std::string GetSymbolValue( API_SYMBOL_KIID const& aParams );
    void SetSymbolValue( API_SYMBOL_VALUE const& aParams );
    std::string GetSymbolFootprint( API_SYMBOL_KIID const& aParams );
    std::string GetSymbolDataSheet( API_SYMBOL_KIID const& aParams );
    void SetSymbolDataSheet( API_SYMBOL_DATASHEET const& aParams );
    std::string GetSymbolDescription( API_SYMBOL_KIID const& aParams );
    void SetSymbolDescription( API_SYMBOL_DESCRIPTION const& aParams );
    std::map<std::string, std::string> GetSymbolFields( API_SYMBOL_KIID const& aParams );
    void AddSymbolField( API_SYMBOL_FIELD const& aParams );
    void RemoveSymbolField( API_SYMBOL_FIELD const& aParams );
    std::vector<API_PIN_INFO> GetSymbolPins( API_SYMBOL_KIID const& aParams );
    API_POINT GetSymbolPinPosition( API_SYMBOL_PIN_INFO const& aParams );
    int GetSymbolPinOrientation( API_SYMBOL_PIN_INFO const& aParams );
    int GetSymbolPinType( API_SYMBOL_PIN_INFO const& aParams );
    void HighlightSymbolPin( API_SYMBOL_PIN_HIGHLIGHT const& aParams );
    void UnhighlightSymbolPin( API_SYMBOL_PIN_INFO const& aParams );
    
    void RotateSymbol( API_ROTATE_SYMBOL const& aParams );

    // Sheet API
    void CreateSheetPin( API_CREATE_SHEET_PIN const& aParams );
    std::string GetRootSheet();
    std::vector<std::string> GetSheetPathList();
    std::string GetSheetPageNumber( API_GET_SHEET_PAGE_NUMBER const& aParams );
    std::string GetSheetByPageNumber( API_GET_SHEET_BY_PAGE_NUMBER const& aParams );
    std::string GetCurrentSheet();

    // Marker API
    void AddMarker( API_ADD_MARKER const& aParams );
    void ClearMarker( API_CLEAR_MARKER const& aParams );
    std::vector<std::string> GetAllMarkers();
    std::string FindMarkerByPosition( API_FIND_MARKER_BY_POSITION const& aParams );

    // Line API
    void CreateBusWireEntry( API_CREATE_BUS_WIRE_ENTRY const& aParams );
    void CreateJunction( API_CREATE_JUNCTION const& aParams );
    std::vector<std::string> GetAllLinesKIID( API_GET_ALL_LINES_KIID const& aParams );
    double GetLineWidth( API_LINE_KIID const& aParams );
    std::string GetWireNetName( API_LINE_KIID const& aParams );
    std::string GetWireDriver( API_LINE_KIID const& aParams );
    void SetLineColor( API_LINE_COLOR const& aParams );
    void SetLineWidth( API_LINE_WIDTH const& aParams );
    void MoveLine( API_MOVE_LINE const& aParams );
    void SetLineStart( API_SET_LINE_START const& aParams );
    void SetLineEnd( API_SET_LINE_END const& aParams );

private:
    SCH_BASE_FRAME* getBaseFrame();
};


#endif