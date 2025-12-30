#ifndef API_HANDLER_PCB_RUN_SDK_API_H
#define API_HANDLER_PCB_RUN_SDK_API_H


#include <cpp_sdk/api_cpp_sdk_param.h>
#include <cpp_sdk/api_cpp_sdk_type.h>

#include <cpp_sdk/api_primitives_pcb_drawer.h>
#include <cpp_sdk/api_cpp_sdk_run_action.h>

class API_CPP_SDK_RUN_ACTION_PCB : public API_CPP_SDK_RUN_ACTION
{
public:
    void runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput ) override;
};

void API_CPP_SDK_RUN_ACTION_PCB::runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
{
    std::string api_name = aParams.api;

    auto api_type = magic_enum::enum_cast<API_CPP_SDK_TYPE>( api_name );

    if( !api_type.has_value() )
    {
        wxLogError( "Failed to enum_cast: %s", api_name );
        return;
    }
    PCB_BASE_EDIT_FRAME*     frame = static_cast<PCB_BASE_EDIT_FRAME*>( aFrame );
    API_PRIMTIVES_PCB_DRAWER api_drawer( frame );
    try
    {
        switch( *api_type )
        {
        case API_CPP_SDK_TYPE::drawPcbTrack:
        {
            const auto& params = aParams.parameter.get<API_PCB_TRACK>();
            api_drawer.DrawPcbTrack( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawMutiLine:
        {
            const auto& params = aParams.parameter.get<API_MULTI_LINES>();
            api_drawer.DrawMultiLines( params );
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
        case API_CPP_SDK_TYPE::drawRectangle:
        {
            const auto& params = aParams.parameter.get<API_RECTANGLE>();
            api_drawer.DrawRectangle( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawBezier:
        {
            const auto& params = aParams.parameter.get<API_BEZIER>();
            api_drawer.DrawBezier( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawSchematicText:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.DrawLabel( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawTextbox:
        {
            const auto& params = aParams.parameter.get<API_TEXTBOX>();
            api_drawer.DrawTextBox( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawTable:
        {
            const auto& params = aParams.parameter.get<API_TABLE>();
            api_drawer.DrawTable( params );
            break;
        }
        case API_CPP_SDK_TYPE::placePcbVia:
        {
            const auto& params = aParams.parameter.get<API_PCB_VIA>();
            api_drawer.PlaceVia( params );
            break;
        }
        case API_CPP_SDK_TYPE::queryLayerNames:
        {
            API_PCB_LAYER_NAME_LIST pcb_layer_names_list;
            api_drawer.GetPcbLayerNamesList( pcb_layer_names_list );
            nlohmann::json json_obj = pcb_layer_names_list;
            aOutput = json_obj.dump();
            break;
        }
        case API_CPP_SDK_TYPE::queryAllFootprintInfo:
        {
            API_PCB_FOOTPRINT_PARAMS_LIST fp_list;
            api_drawer.GetAllPcbFootprintList(fp_list);
            nlohmann::json json_obj = fp_list;
            aOutput = json_obj.dump();
            break;
        }
        case API_CPP_SDK_TYPE::queryFootprintInfo:
        {
            const auto &                   params = aParams.parameter.get<API_PCB_REFERENCE_LIST>();
            API_PCB_FOOTPRINT_PARAMS_LIST fp_list;
            api_drawer.GetPcbFootprintList( params , fp_list);
            nlohmann::json json_obj = fp_list;
            aOutput = json_obj.dump();
            break;
        }
        case API_CPP_SDK_TYPE::moveFootprint:
        {
            const auto& params = aParams.parameter.get<API_MOVE_PCB_FOOTPRINT>();
            api_drawer.MovePcbFootprint( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifyFootprintReference:
        {
            const auto& params = aParams.parameter.get<API_SYMBOL_REFERENCE>();
            api_drawer.ModifyFootprintReference( params );
            break;
        }
        case API_CPP_SDK_TYPE::setFootprintPosition:
        {
            const auto& params = aParams.parameter.get<API_SET_FOOTPRINT_POSITION>();
            api_drawer.SetFootprintPosition( params );
            break;
        }
        case API_CPP_SDK_TYPE::rotateFootprint:
        {
            const auto& params = aParams.parameter.get<API_ROTATE_FOOTPRINT>();
            api_drawer.RotateFootprint( params );
            break;
        }

        default:
            runBaseSdkApi( aFrame, aParams, aOutput );
        }
    }
    catch( const std::exception& e )
    {
        wxLogError( "Failed to Find API : %s,exception :  %s", api_name, e.what() );
    }
    return;
}






#endif