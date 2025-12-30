#ifndef FOOTPRINT_AGENT_RUN_SDK_API_H
#define FOOTPRINT_AGENT_RUN_SDK_API_H


#include <cpp_sdk/api_cpp_sdk_param.h>
#include <cpp_sdk/api_cpp_sdk_type.h>
#include <cpp_sdk/api_primitives_pcb_drawer.h>
#include <footprint_edit_frame.h>
#include <cpp_sdk/api_cpp_sdk_run_action.h>


class API_CPP_SDK_RUN_ACTION_FOOTPRINT : public API_CPP_SDK_RUN_ACTION
{
public:
    void runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput ) override;
};


void API_CPP_SDK_RUN_ACTION_FOOTPRINT::runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
{
    std::string api_name = aParams.api;

    auto api_type = magic_enum::enum_cast<API_CPP_SDK_TYPE>( api_name );

    if( !api_type.has_value() )
    {
        wxLogError( "Failed to enum_cast: %s", api_name );
        return;
    }

    PCB_BASE_EDIT_FRAME* frame = static_cast<PCB_BASE_EDIT_FRAME*>(aFrame);
    API_PRIMTIVES_PCB_DRAWER api_drawer( frame );

    try
    {
        switch( *api_type )
        {
        // case API_CPP_SDK_TYPE::drawWire:
        // {
        //     const auto& params = aParams.parameter.get<API_LINE>();
        //     api_drawer.DrawSingleLine( params );
        //     break;
        // }
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
        case API_CPP_SDK_TYPE::createPcbPad:
        {
            const auto& params = aParams.parameter.get<API_PCB_PAD>();
            api_drawer.CreatePcbPad( params );
            break;
        }
        case API_CPP_SDK_TYPE::movePcbPad:
        {
            const auto& params = aParams.parameter.get<API_MOVE_PCB_PAD>();
            api_drawer.MovePcbPad( params );
            break;
        }
        case API_CPP_SDK_TYPE::rotatePcbPad:
        {
            const auto& params = aParams.parameter.get<API_ROTATE_PCB_PAD>();
            api_drawer.RotatePcbPad( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifyPadNumber:
        {
            const auto& params = aParams.parameter.get<API_SET_PAD_NUMBER>();
            api_drawer.SetPcbPadNumber( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifyPadSize:
        {
            const auto& params = aParams.parameter.get<API_SET_PAD_SIZE>();
            api_drawer.SetPcbPadSize( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifyPadDrillSize:
        {
            const auto& params = aParams.parameter.get<API_SET_PAD_SIZE>();
            api_drawer.SetPcbPadDrillSize( params );
            break;
        }
        case API_CPP_SDK_TYPE::modifyPadDrillShape:
        {
            const auto& params = aParams.parameter.get<API_SET_PAD_DRILL_SHAPE>();
            api_drawer.SetPcbPadDrillType( params );
            break;
        }
        case API_CPP_SDK_TYPE::setPadPosition:
        {
            const auto& params = aParams.parameter.get<API_SET_PAD_POSITION>();
            api_drawer.SetPcbPadPosition( params );
            break;
        }
        default: runBaseSdkApi( aFrame, aParams, aOutput );
        }
    }
    catch( const std::exception& e )
    {
        wxLogError( "Footprint Editor Frame Failed to Find API : %s,exception :  %s", api_name, e.what() );
    }

    return;
}


#endif