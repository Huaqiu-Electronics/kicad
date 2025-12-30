#include <vector>
#include <string>
#include "pcb_primitives_net.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_NET::GetAllNetName(std::vector<std::string>& nets)
{
    nets.clear();
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS !=  status || !board )
    {
        return status;
    }
    for(NETINFO_ITEM* net : board->GetNetInfo())
    {
        //NETCLASS* nc = net->GetNetClass();
        std::string aName = net->GetNetname().ToStdString();
        nets.push_back(aName);
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    
}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_NET::GetNetCodeByNetName(const std::string& netName, int& netCode)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS !=  status || !board )
    {
        return status;
    }
    NETINFO_ITEM* netinfo = board->FindNet( netName );
    if( netinfo )
    {
        netCode = netinfo->GetNetCode();
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;           
}