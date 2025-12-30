#ifndef SYMBOL_AGENT_RUN_SDK_API_H
#define SYMBOL_AGENT_RUN_SDK_API_H
#include <symbol_edit_frame.h>
#include <cpp_sdk/api_cpp_sdk_param.h>
#include <cpp_sdk/api_cpp_sdk_type.h>
#include <cpp_sdk/api_primitive_sch_drawer.h>

#include <cpp_sdk/api_cpp_sdk_run_action.h>
class API_CPP_SDK_RUN_ACTION_SYMBOL : public API_CPP_SDK_RUN_ACTION
{
public:
    void runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput ) override;
};

void API_CPP_SDK_RUN_ACTION_SYMBOL::runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
{
    std::string api_name = aParams.api;

    auto api_type = magic_enum::enum_cast<API_CPP_SDK_TYPE>( api_name );

    if( !api_type.has_value() )
    {
        wxLogError( "Failed to enum_cast: %s", api_name );
        return;
    }

    SCH_BASE_FRAME*          frame = static_cast<SCH_BASE_FRAME*>( aFrame );
    API_PRIMITIVE_SCH_DRAWER api_drawer( frame );

    try
    {
        switch( *api_type )
        {
        case API_CPP_SDK_TYPE::drawWire:
        {
            const auto& params = aParams.parameter.get<API_LINE>();
            api_drawer.DrawWire( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawMultiWire:
        case API_CPP_SDK_TYPE::drawBus:
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
        case API_CPP_SDK_TYPE::drawBezier:
        {
            const auto& params = aParams.parameter.get<API_BEZIER>();
            api_drawer.DrawBezier( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawRectangle:
        {
            const auto& params = aParams.parameter.get<API_RECTANGLE>();
            api_drawer.DrawRectangle( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawTextbox:
        {
            const auto& params = aParams.parameter.get<API_TEXTBOX>();
            api_drawer.DrawTextbox( params );
            break;
        }
        case API_CPP_SDK_TYPE::drawSchematicText:
        {
            const auto& params = aParams.parameter.get<API_LABEL>();
            api_drawer.DrawSchematicText( params );
            break;
        }
        case API_CPP_SDK_TYPE::addSymbolLibrary:
        {
            const auto& params = aParams.parameter.get<API_CREATE_SYMBOL_LIBRARY>();
            api_drawer.CreateSymbolLibrary( params );
            break;
        }
        case API_CPP_SDK_TYPE::addLibSymbolPin:
        {
            const auto& params = aParams.parameter.get<API_LIB_SYMBOL_PIN>();
            api_drawer.CreateLibSymbolPin( params );
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

