#ifndef PCB_NET_H
#define PCB_NET_H

#include <string>
#include <vector>

enum class KI_SDK_EXEC_RESULT : int;

class PCB_PRIMITIVES_NET
{
public:
    KI_SDK_EXEC_RESULT      GetAllNetName(std::vector<std::string>& nets);

    KI_SDK_EXEC_RESULT      GetNetCodeByNetName(const std::string& netName, int& netCode);
};


#endif