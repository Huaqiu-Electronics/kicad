#ifndef __MCP_STATUS_MSG_H__
#define __MCP_STATUS_MSG_H__

#include <nlohmann/json.hpp>
#include "mcp_status.h"

struct MCP_STATUS_MSG
{
    MCP_STATUS  code;
    std::string msg;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MCP_STATUS_MSG, code, msg )
};


#endif // __MCP_STATUS_MSG_H__