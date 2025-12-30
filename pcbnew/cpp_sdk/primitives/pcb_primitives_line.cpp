#include <pcb_shape.h>
#include <board_design_settings.h>

#include <cpp_sdk/ki_sdk_result_codes.h>
#include <board_commit.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include "pcb_primitives_line.h"

PCB_PRIMITIVES_LINE::PCB_PRIMITIVES_LINE( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}

KI_SDK_EXEC_RESULT FindLineByKIID(const std::string& aKID, PCB_SHAPE*& line)
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !ctx.IsValid())
        return status;
    BOARD* board = ctx.board;
    for(auto& item : board->Drawings())
    {
        if(item->Type() != PCB_SHAPE_T)
            continue;
        if(item->m_Uuid.AsStdString() != aKID)
            continue;
        PCB_SHAPE* shape = static_cast<PCB_SHAPE*>(item);
        if(shape->GetShape() != SHAPE_T::SEGMENT)
            continue;
        line = shape;
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_LINE::CreateLine( const VECTOR2I& start, const VECTOR2I& end, PCB_LAYER_ID layer,
                                                    int lineWidth, bool lock, std::string& aKID )
{

    STROKE_PARAMS                stroke;
    const BOARD_DESIGN_SETTINGS& bds = GetBaseEditFrame()->GetDesignSettings();

    stroke.SetWidth( bds.GetLineThickness( layer ) );
    stroke.SetLineStyle( LINE_STYLE::DEFAULT );
    stroke.SetColor( COLOR4D::UNSPECIFIED );

    PCB_SHAPE* line = new PCB_SHAPE( GetBoard() );

    line->SetShape( SHAPE_T::SEGMENT );
    line->SetFlags( IS_NEW );
    line->SetStart( start );
    line->SetEnd( end );
    line->SetLayer( layer );
    line->SetStroke( stroke );
    line->SetFilled( false );
    line->SetLocked( lock );

    if( lineWidth != 0 )
    {
        line->SetWidth( lineWidth );
    }

    aKID = line->m_Uuid.AsStdString();

    GetBoard()->Add( line );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( line );

    aKID = line->m_Uuid.AsStdString();

    GetBaseEditFrame()->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetAllLinesKIID(std::vector<std::string>&  allKIDList )
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || board == nullptr)
        return status;
    for(auto& item : board->Drawings())
    {
        if(item->Type() != PCB_SHAPE_T)
            continue;
        PCB_SHAPE* shape = static_cast<PCB_SHAPE*>(item);
        if(shape->GetShape() != SHAPE_T::SEGMENT)
            continue;
        allKIDList.push_back(item->m_Uuid.AsStdString());
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineWidth(const std::string& aKID, int& width)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    width = line->GetWidth();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}



KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineLayer(const std::string& aKID, PCB_LAYER_ID& layer)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    layer = line->GetLayer();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineLayerName(const std::string& aKID, std::string& layerName)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    layerName = line->GetLayerName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineStart(const std::string& aKID, VECTOR2I& start)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    start = line->GetStart();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineEnd(const std::string& aKID, VECTOR2I& end)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    end = line->GetEnd();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::GetLineLockState(const std::string& aKID, bool& lock)
{
    PCB_SHAPE* line = nullptr;
    KI_SDK_EXEC_RESULT status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    lock = line->IsLocked();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::SetLineWidth(const std::string& aKID, int width)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !frame )
        return status;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    line->SetWidth(width);
    frame->GetCanvas()->GetView()->Update(line);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::SetLineLayer(const std::string& aKID, PCB_LAYER_ID layer)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !frame )
        return status;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    line->SetLayer(layer);
    frame->GetCanvas()->GetView()->Update(line);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::SetLineStart(const std::string& aKID,const VECTOR2I& start)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !frame )
        return status;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    line->SetStart(start);
    frame->GetCanvas()->GetView()->Update(line);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::SetLineEnd(const std::string& aKID,const VECTOR2I& end)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !frame )
        return status;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    line->SetEnd(end);
    frame->GetCanvas()->GetView()->Update(line);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::SetLineLockState(const std::string& aKID, bool lock)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !frame )
        return status;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    line->SetLocked(lock);
    frame->GetCanvas()->GetView()->Update(line);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::DeleteAllLines()
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !ctx.IsValid())
        return status;
    PCB_EDIT_FRAME* frame = ctx.frame;
    BOARD* board = ctx.board;
    std::vector<PCB_SHAPE*> allLines;
    for(auto& item : board->Drawings())
    {
        if(item->Type() != PCB_SHAPE_T)
            continue;
        PCB_SHAPE* shape = static_cast<PCB_SHAPE*>(item);
        if(shape->GetShape() != SHAPE_T::SEGMENT)
            continue;
        allLines.push_back(shape);
    }
    for(auto& line : allLines)
    {
        frame->GetCanvas()->GetView()->Remove( line );
        board->Remove( line );
        delete line;
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT      PCB_PRIMITIVES_LINE::DeleteLineByKIID(const std::string& aKID)
{
    PcbUtilsContext ctx{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&ctx);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !ctx.IsValid())
        return status;
    PCB_EDIT_FRAME* frame = ctx.frame;
    BOARD* board = ctx.board;
    PCB_SHAPE* line = nullptr;
    status = FindLineByKIID(aKID, line);
    if (KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !line )
        return status;
    frame->GetCanvas()->GetView()->Remove( line );
    board->Remove( line );
    delete line;
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}
