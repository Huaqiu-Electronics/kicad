#ifndef FOOTPRINT_AGENT_RUN_ACTION_H
#define FOOTPRINT_AGENT_RUN_ACTION_H


#include <passive_action/agent/agent_action_type.h>
#include <cpp_sdk/api_cpp_sdk_param.h>

#include <footprint_edit_frame.h>
#include "footprint_agent_run_sdk_api.h"

template <>
void FOOTPRINT_COPILOT_PANEL_CONTAINER::ExecuteAgentAction( AGENT_ACTION const& aAction )
{
    try
    {
        auto t = magic_enum::enum_cast<AGENT_ACTION_TYPE>( aAction.action );

        std::string jsonstr = nlohmann::json( aAction ).dump();

        if( !t.has_value() )
        {
            wxLogWarning( "FOOTPRINT_COPILOT_PANEL_CONTAINER Unknown action received: %s", aAction.action.c_str() );
            return;
        }

        switch( *t )
        {
        case AGENT_ACTION_TYPE::cpp_sdk_api:
        {
            auto        content = aAction.context.get<CPP_SDK_API_PARAM>();
            std::string output;
            // runSdkApi_fp( self(), content, output );
            API_CPP_SDK_RUN_ACTION_FOOTPRINT fp_agent;
            fp_agent.runSdkApi( self(), content, output );
            break;
        }

        default: return BaseExecuteAgentAction( aAction );
        }
    }
    catch( std::exception const& e )
    {
        wxLogError( "FOOTPRINT_COPILOT_PANEL_CONTAINER Exception caught: %s while processing action %s:", e.what(),
                    nlohmann::json( aAction ).dump().c_str() );
    }
}





template <>
void FOOTPRINT_COPILOT_PANEL_CONTAINER::ExecuteSdkQuerAgentAction( AGENT_ACTION const& aAction, std::string& agentResult )
{
    try
    {
        auto t = magic_enum::enum_cast<AGENT_ACTION_TYPE>( aAction.action );

        if( !t.has_value() )
        {
            wxLogWarning( "Unknown action received: %s", aAction.action.c_str() );
            return;
        }

        switch( *t )
        {
            case AGENT_ACTION_TYPE::cpp_sdk_api:
        {
            auto content = aAction.context.get<CPP_SDK_API_PARAM>();
            API_CPP_SDK_RUN_ACTION_FOOTPRINT fp_action;
            fp_action.runSdkApi( self(), content, agentResult );
            break;
        }

        default: return BaseExecuteAgentAction( aAction );
        }
    }
    catch( std::exception const& e )
    {
        wxLogError( "Exception caught: %s while processing action %s:", e.what(),
                    nlohmann::json( aAction ).dump().c_str() );
    }
    return;
}





#endif