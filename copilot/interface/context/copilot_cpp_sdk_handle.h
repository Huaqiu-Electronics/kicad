#ifndef COPILOT_CPP_SDK_HANDLE_H
#define COPILOT_CPP_SDK_HANDLE_H

#include <functional>
#include <memory>
#include <string>

struct AGENT_ACTION;

using CPP_SDK_AGENT_ACTION_HANDLE_T = std::function<void( AGENT_ACTION const& , std::string&)>;
using CPP_SDK_AGENT_ACTION_HANDLE = std::weak_ptr<CPP_SDK_AGENT_ACTION_HANDLE_T>;
using CPP_SDK_AGENT_ACTION_OWNED_HANDLE = std::shared_ptr<CPP_SDK_AGENT_ACTION_HANDLE_T>;

#endif