#ifndef SCH_CPP_SDK_QUERY_ACTION_EXECUTOR_H
#define SCH_CPP_SDK_QUERY_ACTION_EXECUTOR_H

#include <sch_edit_frame.h>
#include <api/api_handler_sch.h>
#include <passive_action/agent/agent_action.h>
#include <passive_action/agent/agent_action_type.h>

#include <cpp_sdk/api_cpp_sdk_param.h>
#include <cpp_sdk/api_handler_sch_run_sdk_api.h>

template<>
void SCH_COPILOT_PANEL_CONTAINER::ExecuteSdkQueryAgentAction( AGENT_ACTION const& aAction , std::string& agentResult )
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
            API_CPP_SDK_RUN_ACTION_SCH sch_action;
            sch_action.runSdkApi( self(), content, agentResult );
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