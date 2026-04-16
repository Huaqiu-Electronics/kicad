#ifndef API_HANDLER_SCH_RUN_SDK_API_H
#define API_HANDLER_SCH_RUN_SDK_API_H
// #include "api_handler_sch.h"
#include <cpp_sdk/cpp_sdk_sch_api.h>
#include <layer_ids.h>
#include <cpp_sdk/api_cpp_sdk_type.h>
#include <cpp_sdk/api_primitive_sch_drawer.h>
#include <nlohmann/json.hpp>

#include <cpp_sdk/api_cpp_sdk_run_action.h>

class API_CPP_SDK_RUN_ACTION_SCH : public API_CPP_SDK_RUN_ACTION
{
public:
    void runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput ) override;
};

void API_CPP_SDK_RUN_ACTION_SCH::runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
{
    std::string api_name = aParams.api;

    auto api_type = magic_enum::enum_cast<API_CPP_SDK_TYPE>( api_name );

    if( !api_type.has_value() )
    {
        wxLogError( "Failed to enum_cast: %s", api_name );
        return;
    }

    SCH_EDIT_FRAME*          frame = static_cast<SCH_EDIT_FRAME*>( aFrame );
    API_PRIMITIVE_SCH_DRAWER api_drawer( frame );

    try
    {
        switch( *api_type )
        {
        case API_CPP_SDK_TYPE::drawMutiLine:
        {
            const auto& params = aParams.parameter.get<API_MULTI_LINES>();
            api_drawer.DrawMultiLines( params );

            break;
        }
        case API_CPP_SDK_TYPE::drawWire:
        {
            const auto& params = aParams.parameter.get<API_LINE>();
            api_drawer.DrawWire( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawMultiWire:
        {
            const auto& params = aParams.parameter.get<API_MULTI_LINES>();
            api_drawer.DrawMultiWires( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawBus:
        {
            const auto& params = aParams.parameter.get<API_MULTI_LINES>();
            api_drawer.DrawBus( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawCircle:
        {
            const auto& params = aParams.parameter.get<API_CIRCLE>();
            api_drawer.DrawCircle( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawArc:
        {
            const auto& params = aParams.parameter.get<API_ARC>();
            api_drawer.DrawArc( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawBezier:
        {
            const auto& params = aParams.parameter.get<API_BEZIER>();
            api_drawer.DrawBezier( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawTextbox:
        {
            const auto& params = aParams.parameter.get<API_TEXTBOX>();
            api_drawer.DrawTextbox( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawTable:
        {
            const auto& params = aParams.parameter.get<API_TABLE>();
            api_drawer.DrawTable( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawRectangle:
        {
            const auto& params = aParams.parameter.get<API_RECTANGLE>();
            api_drawer.DrawRectangle( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawSchematicText:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.DrawSchematicText( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawHierSheet:
        {
            const auto& params = aParams.parameter.get<API_HIER_SHEET>();
            api_drawer.DrawHierSheet( params );
            break;
        }
        case API_CPP_SDK_TYPE::placeLocalLabel:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.PlaceLocalLabel( params );
            break;
        }
        case API_CPP_SDK_TYPE::placeGlobalLabel:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.PlaceGlobalLabel( params );
            break;
        }
        case API_CPP_SDK_TYPE::placeHierLabel:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.PlaceHierLabel( params );
            break;
        }
        case API_CPP_SDK_TYPE::placeClassLabel:
        {
            const auto& params = aParams.parameter.get<API_CLASS_LABEL>();
            api_drawer.PlaceClassLabel( params );
            break;
        }
        case API_CPP_SDK_TYPE::querySymbolPin:
        {
            const auto&          params = aParams.parameter.get<API_QUERY_SYMBOL>();
            API_SYMBOL_PINS_INFO symbol_pins_info;
            api_drawer.GetSymbolPinsInfo( params, symbol_pins_info );
            nlohmann::json json_obj = symbol_pins_info;
            aOutput = json_obj.dump();
            break;
        }
        case API_CPP_SDK_TYPE::placeSymbol:
        {
            const auto& params = aParams.parameter.get<API_PLACE_SYMBOL>();
            api_drawer.PlaceSymbol( params );
            break;
        }
        case API_CPP_SDK_TYPE::getSymbolLibrary:
        {
            API_SYMBOL_LIBRARY_LIST library_list;
            api_drawer.GetSymbolLibrary( library_list );
            nlohmann::json json_obj = library_list;
            aOutput = json_obj.dump();
            break;
        }
        case API_CPP_SDK_TYPE::moveSymbol:
        {
            const auto& params = aParams.parameter.get<API_MOVE_SYMBOL>();
            api_drawer.MoveSymbol( params );
            break;
        }
        case API_CPP_SDK_TYPE::rotateSymbol:
        {
            const auto& params = aParams.parameter.get<API_ROTATE_SYMBOL>();
            api_drawer.RotateSymbol( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifySymbolValue:
        {
            const auto& params = aParams.parameter.get<API_SYMBOL_VALUE>();
            api_drawer.SetSymbolValue( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifySymbolReference:
        {
            const auto& params = aParams.parameter.get<API_SYMBOL_REFERENCE>();
            api_drawer.SetSymbolReference( params );
            break;
        }
        case API_CPP_SDK_TYPE::importVectorGraphic:
        {
            api_drawer.ImportVectorGraphicFile();
            break;
        }
        case API_CPP_SDK_TYPE::exportNetlist:
        {
            api_drawer.ExportNetlist();
            break;
        }
        case API_CPP_SDK_TYPE::schematicSetup:
        {
            api_drawer.OpenSchematicSetupDlg();
            break;
        }
        case API_CPP_SDK_TYPE::editTextGraphicProperty:
        {
            api_drawer.OpenEditTextAndGraphicPropertyDlg();
            break;
        }
        case API_CPP_SDK_TYPE::hierarchyPanel:
        {
            api_drawer.ToggleHierarchyPanel();
            break;
        }
        case API_CPP_SDK_TYPE::netNavigatorPanel:
        {
            api_drawer.ToggleNetNavigatorPanel();
            break;
        }
        case API_CPP_SDK_TYPE::designBlockPanel:
        {
            api_drawer.ToggleDesignBlockPanel();
            break;
        }
        case API_CPP_SDK_TYPE::symbolLibraryBrowser:
        {
            api_drawer.OpenSymbolLibraryBrowser();
            break;
        }
        case API_CPP_SDK_TYPE::showBusSyntaxHelp:
        {
            api_drawer.ShowBusSynatxHelp();
            break;
        } 
        case API_CPP_SDK_TYPE::runERC:
        {
            api_drawer.RunERC();
            break;
        }
        case API_CPP_SDK_TYPE::showSimulator:
        {
            api_drawer.ShowSimulator();
            break;
        }
        default: runBaseSdkApi( aFrame, aParams, aOutput );
        }
    }
    catch( const std::exception& e )
    {
        wxLogError( "Failed to Find API : %s,exception :  %s", api_name, e.what() );
    }

    return;
}


#endif