#include <pcb_track.h>
#include <board_item.h>
#include <netinfo.h>
#include <math/vector2d.h>
#include <layer_ids.h>
#include "pcb_primitives_via.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include <pcb_group.h>
 

KI_SDK_EXEC_RESULT  FindViaByKIID(const std::string& aKID, PCB_VIA*& via)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(status != KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS || !board)
    {
        return status;
    }
    KIID  aID = KIID(aKID);
    for( PCB_TRACK* track : board->Tracks() )
    {
        if( PCB_VIA_T == track->Type() && track->m_Uuid == aID)
        {
            via = static_cast<PCB_VIA*>(track);
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}



KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::CreateVia(int netCode, const VECTOR2I& position, VIATYPE& aType, PCB_LAYER_ID& first_layer, PCB_LAYER_ID& last_layer, int drill, int width,  bool lock, std::string& aKID)
{
    PcbUtilsContext context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status ||  !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;
    BOARD*   board = context.board;

    PCB_VIA*  via = nullptr;
    via = new PCB_VIA(board);
    via->SetPosition(position);
    via->SetViaType(aType);

    if(via->GetViaType() == VIATYPE::MICROVIA)
    {
        via->SetWidth(PADSTACK::ALL_LAYERS,
                        via->GetEffectiveNetClass()->GetuViaDiameter());
        via->SetDrill(via->GetEffectiveNetClass()->GetuViaDrill());
    }else
    {

        via->SetWidth(PADSTACK::ALL_LAYERS, width);
        via->SetDrill(drill);
    }
    via->SetLayerPair(first_layer, last_layer);

    via->SetNetCode(netCode);
    if(lock)
    {
        via->SetLocked(true);
    }

    board->Add(via);
    frame->GetCanvas()->GetView()->Add( via );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}



KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::GetAllViaKIID(std::vector<std::string>& allKID)
{
    allKID.clear();
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(status != KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS || !board)
    {
        return status;
    }
    for(PCB_TRACK* item : board->Tracks())
    {
        if( PCB_VIA_T == item->Type())
        {
            allKID.push_back(item->m_Uuid.AsStdString());
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_VIA::GetViaPosition(const std::string& aKID, VECTOR2I& position)
{
    PCB_VIA*  via = nullptr;
    KI_SDK_EXEC_RESULT  status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    position = via->GetPosition();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_VIA::GetViaType(const std::string& aKID, VIATYPE& viaType)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    viaType = via->GetViaType();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}



KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_VIA::SetViaPosition(const std::string& aKID, const VECTOR2I& position)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return status;
    }
    PCB_VIA* via = nullptr;
    status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }

    via->SetPosition(position);
    frame->GetCanvas()->GetView()->Update(via);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::SetViaType(const std::string& aKID, const VIATYPE& viaType)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    via->SetViaType(viaType);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::GetViaDrill(const std::string& aKID, int& drill)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    drill = via->GetDrill();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::SetViaDrill(const std::string& aKID, int drill)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    via->SetDrill(drill);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::GetViaWidth(const std::string& aKID,const PCB_LAYER_ID& aLayer, int& width)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT  status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    width = via->GetWidth(aLayer);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::GetViaLockState(const std::string& aKID, bool& lock)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT  status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    lock = via->IsLocked();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_VIA::SetViaLocked(const std::string& aKID, bool lock)
{
    PCB_VIA* via = nullptr;
    KI_SDK_EXEC_RESULT  status = FindViaByKIID(aKID, via);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }
    via->SetLocked(lock);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_VIA::DeleteViaByKIID(const std::string& aKID)
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);

    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !ctx.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = ctx.frame;
    BOARD* board = ctx.board;

    PCB_VIA* via = nullptr;
    status = FindViaByKIID( aKID, via );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !via)
    {
        return status;
    }

    if( auto group = via->GetParentGroup() )
    {
        group->RemoveItem( via );
    }
    board->Remove(via);
    frame->GetCanvas()->GetView()->Remove(via);
    delete via;
    
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}
