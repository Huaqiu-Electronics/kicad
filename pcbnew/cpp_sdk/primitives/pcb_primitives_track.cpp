#include <pcb_track.h>
#include <board_item.h>
#include <netinfo.h>
#include <math/vector2d.h>
#include <layer_ids.h>
#include "pcb_primitives_track.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include <pcb_group.h>
#include <connectivity/connectivity_data.h>
#include <vector>
#include <string>

KI_SDK_EXEC_RESULT  FindTrackByKIID(const std::string& aKID, PCB_TRACK*& track)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(status != KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS || !board)
    {
        return status;
    }
    KIID  aID = KIID(aKID);
    for( PCB_TRACK* item : board->Tracks() )
    {
        if( item->IsType( { PCB_TRACE_T, PCB_ARC_T } ) && item->m_Uuid == aID)
        {
            track = item;
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::CreateTrack(int netCode, const VECTOR2I& start, const VECTOR2I& end, PCB_LAYER_ID layer, int lineWidth, bool lock, std::string& aKID)
{
    PcbUtilsContext context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status ||  !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;
    BOARD*   board = context.board;
    PCB_TRACK*  track = new PCB_TRACK( board ); 
    if(!track)
    {
        aKID = "";
        return KI_SDK_EXEC_RESULT::KI_SDK_INTERNAL_ERROR;
    }
    track->SetStart(start);
    track->SetEnd(end);

    if( layer != PCB_LAYER_ID::UNDEFINED_LAYER )
    {
        track->SetLayer( layer );
    }else
    {
        
        track->SetLayer( frame->GetActiveLayer());
    }
        
    track->SetWidth(lineWidth);
    track->SetNetCode(netCode);
    if(lock)
    {
        track->SetLocked(true);
    }

    board->Add(track);
    frame->GetCanvas()->GetView()->Add( track );

    aKID = track->m_Uuid.AsStdString();

    frame->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}



KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::CreateArcTrack(int netCode, const VECTOR2I& start, const VECTOR2I& mid, const VECTOR2I& end, PCB_LAYER_ID layer, int lineWidth, bool lock, std::string& aKID)
{
    PcbUtilsContext context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status ||  !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;
    BOARD*   board = context.board;
    PCB_ARC*  arc = new PCB_ARC( board ); 
    if(!arc)
    {
        aKID = "";
        return KI_SDK_EXEC_RESULT::KI_SDK_INTERNAL_ERROR;
    }
    arc->SetStart(start);
    arc->SetEnd(end);
    arc->SetMid(mid);
    arc->SetLayer(layer);
    arc->SetWidth(lineWidth);
    arc->SetNetCode(netCode);
    if(lock)
    {
        arc->SetLocked(true);
    }

    board->Add(arc);
    frame->GetCanvas()->GetView()->Add( arc );

    aKID = arc->m_Uuid.AsStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::GetAllTracksKIID(std::vector<std::string>&  allKIDList )
{
    allKIDList.clear();
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT  status = GetPcbBoard( board );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !board)
    {
        return status;
    }
    for(auto& track : board->Tracks())
    {
        if( !track->IsType( { PCB_TRACE_T, PCB_ARC_T } ) )
            continue;
        allKIDList.push_back(track->m_Uuid.AsStdString());
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::GetTrackWidth(const std::string& aKID, int& width)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    width = track->GetWidth();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackLayer(const std::string& aKID, PCB_LAYER_ID& layer)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    layer = track->GetLayer();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackLayerName(const std::string& aKID, std::string& layerName)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    layerName = track->GetLayerName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackNetCode(const std::string& aKID, int& netCode)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    netCode = track->GetNetCode();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackNetName(const std::string& aKID, std::string& netName)
{
    PCB_TRACK*  track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    netName = track->GetNetname().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackStart(const std::string& aKID, VECTOR2I& start)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    start = track->GetStart();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackEnd(const std::string& aKID, VECTOR2I& end)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    end = track->GetEnd();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::GetTrackMid(const std::string& aKID, VECTOR2I& mid)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    if( PCB_TRACE_T == track->Type() )
    {
       mid = ( track->GetStart() + track->GetEnd() ) / 2;
    }else if(PCB_ARC_T == track->Type())
    {
        PCB_ARC* arc = static_cast<PCB_ARC*>(track);
        mid = arc->GetMid();
    }else
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::GetTrackLockState(const std::string& aKID, bool& lock)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    lock = track->IsLocked();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackNetCode(const std::string& aKID, int netCode)
{
    PCB_TRACK* track = nullptr;
    KI_SDK_EXEC_RESULT  status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    track->SetNetCode(netCode);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackWidth(const std::string& aKID, int width)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame( frame );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    track->SetWidth(width);

    frame->GetCanvas()->GetView()->Update(track);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackLayer(const std::string& aKID, PCB_LAYER_ID layer)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame( frame );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    track->SetLayer( layer );
    frame->GetCanvas()->GetView()->Update(track);

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackStart(const std::string& aKID,const VECTOR2I& start)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT  status = GetPcbEditFrame( frame );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return status;
    }
    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    track->SetStart(start);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::DeleteAllTracks()
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);

    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !ctx.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = ctx.frame;
    BOARD* board = ctx.board;
    for( PCB_TRACK* track : board->Tracks() )
    {
        frame->GetCanvas()->GetView()->Remove(track);
    }
    TRACKS tracks = board->Tracks();
    board->RemoveAll({PCB_TRACE_T});
    for( PCB_TRACK* track : tracks )
    {
        if( PCB_GROUP* group = track->GetParentGroup() )
        {
            group->RemoveItem( track );
        }
        delete track;
    }

    board->GetConnectivity()->ClearRatsnest();
    board->BuildConnectivity();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_TRACK::DeleteTrackByKIID(const std::string& aKID)
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);

    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !ctx.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = ctx.frame;
    BOARD* board = ctx.board;

    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID( aKID, track );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }

    if( PCB_GROUP* group = track->GetParentGroup() )
    {
        group->RemoveItem( track );
    }
    board->Remove(track);
    frame->GetCanvas()->GetView()->Remove(track);
    delete track;
    
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackEnd(const std::string& aKID,const VECTOR2I& end)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT  status = GetPcbEditFrame( frame );
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return status;
    }
    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track)
    {
        return status;
    }
    track->SetEnd(end);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_TRACK::SetTrackLockState(const std::string& aKID, bool lock)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard( board );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !board )
    {
        return status;
    }
    PCB_TRACK* track = nullptr;
    status = FindTrackByKIID(aKID, track);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !track )
    {
        return status;
    }
    track->SetLocked( lock );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}




