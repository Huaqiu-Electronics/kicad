#ifndef SYMBOL_AGENT_RUN_ACTION_H
#define SYMBOL_AGENT_RUN_ACTION_H

#include <passive_action/agent/agent_action_type.h>
#include <cpp_sdk/api_cpp_sdk_param.h>
#include <symbol_edit_frame.h>
#include "symbol_agent_run_sdk_api.h"

template <>
void SYMBOL_COPILOT_PANEL_CONTAINER::ExecuteAgentAction( AGENT_ACTION const& aAction )
{
    try
    {
        auto t = magic_enum::enum_cast<AGENT_ACTION_TYPE>( aAction.action );

        std::string jsonstr = nlohmann::json( aAction ).dump();

        if( !t.has_value() )
        {
            wxLogWarning( "SYMBOL_COPILOT_PANEL_CONTAINER Unknown action received: %s", aAction.action.c_str() );
            return;
        }

        switch( *t )
        {
        case AGENT_ACTION_TYPE::cpp_sdk_api:
        {
            auto        content = aAction.context.get<CPP_SDK_API_PARAM>();
            std::string output;
            API_CPP_SDK_RUN_ACTION_SYMBOL symbol_action;
            symbol_action.runSdkApi( self(), content, output );
            break;
        }

        default: return BaseExecuteAgentAction( aAction );
        }
    }
    catch( std::exception const& e )
    {
        wxLogError( "SYMBOL_COPILOT_PANEL_CONTAINER Exception caught: %s while processing action %s:", e.what(),
                    nlohmann::json( aAction ).dump().c_str() );
    }
}


template <>
void SYMBOL_COPILOT_PANEL_CONTAINER::ExecuteSdkQuerAgentAction( AGENT_ACTION const& aAction, std::string& agentResult )
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
            auto                       content = aAction.context.get<CPP_SDK_API_PARAM>();
            API_CPP_SDK_RUN_ACTION_SYMBOL symbol_action;
            symbol_action.runSdkApi( self(), content, agentResult );
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