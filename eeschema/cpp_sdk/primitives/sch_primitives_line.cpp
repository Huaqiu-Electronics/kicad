#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_line.h>
#include <layer_ids.h>
#include <sch_bus_entry.h>
#include <sch_junction.h>
#include <sch_shape.h>
#include "sch_primitives_line.h"

SCH_PRIMITIVES_LINE::SCH_PRIMITIVES_LINE( SCH_BASE_FRAME* aFrame ) : SCH_PRIMITIVES_ITEM( aFrame )
{
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::CreateLine( const VECTOR2I& start, const VECTOR2I& end, SCH_LAYER_ID layer,
                                                    int lineWidth, bool lock, std::string& aKID )
{
    if( FRAME_T::FRAME_SCH == GetBaseFrame()->GetFrameType() )
    {
        return createLineInSchEditFrame( start, end, layer, lineWidth, lock, aKID );
    }
    else if( FRAME_T::FRAME_SCH_SYMBOL_EDITOR == GetBaseFrame()->GetFrameType() )
    {
        return createLineInSymbolFrame( start, end, layer, lineWidth, lock, aKID );
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::createLineInSymbolFrame( const VECTOR2I& start, const VECTOR2I& end,
                                                                 SCH_LAYER_ID layer, int lineWidth, bool lock,
                                                                 std::string& aKID )
{
    if( start == end )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    SCH_SHAPE* sch_line = new SCH_SHAPE( SHAPE_T::POLY, GetLayerId(), lineWidth, FILL_T::NO_FILL );
    sch_line->SetParent( GetItemParent() );
    sch_line->SetFillColor( COLOR4D::UNSPECIFIED );
    sch_line->SetFlags( IS_NEW );

    sch_line->BeginEdit( start );
    sch_line->CalcEdit( end );
    sch_line->EndEdit( true );
    sch_line->ClearEditFlags();


    aKID = sch_line->m_Uuid.AsStdString();

    // frame->AddToScreen( segment, frame->GetScreen() );

    AddItemToScreen( sch_line );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::createLineInSchEditFrame( const VECTOR2I& start, const VECTOR2I& end,
                                                                  SCH_LAYER_ID layerId, int lineWidth, bool lock,
                                                                  std::string& aKID )
{
    if( start == end )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    SCH_LINE* segment = new SCH_LINE( start, layerId );
    segment->SetParent( GetItemParent() );
    segment->SetFlags( IS_NEW );

    segment->SetStartPoint( start );
    segment->SetEndPoint( end );

    segment->SetLocked( lock );

    if( lineWidth != 0 )
    {
        segment->SetLineWidth( lineWidth );
    }

    segment->ClearEditFlags();
    aKID = segment->m_Uuid.AsStdString();

    // frame->AddToScreen( segment, frame->GetScreen() );

    AddItemToScreen( segment );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}




///
KI_SDK_EXEC_RESULT FindLineByKIID( const std::string& aKID, SCH_LINE*& aLine );
KI_SDK_EXEC_RESULT FindLineByKIID( const std::string& aKID, SCH_LINE*& aLine, SCH_SHEET_PATH& aSheetPath );

KI_SDK_EXEC_RESULT FindLineByKIID( const std::string& aKID, SCH_LINE*& aLine )
{
    SCH_SHEET_PATH emptySheetPath;
    return FindLineByKIID( aKID, aLine, emptySheetPath );
}

//查找当前Screen下的line
KI_SDK_EXEC_RESULT FindLineByKIID( const std::string& aKID, SCH_LINE*& aLine, SCH_SHEET_PATH& aSheetPath )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );
    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    // SCH_SHEET_LIST sheets = frame->Schematic().Hierarchy();
    for( SCH_SHEET_PATH& sheet : frame->Schematic().Hierarchy() )
    {
        SCH_SCREEN* screen = sheet.LastScreen();
        for( auto& elem : screen->Items().OfType( SCH_LINE_T ) )
        {
            if( elem->m_Uuid.AsStdString() == aKID )
            {
                aLine = static_cast<SCH_LINE*>( elem );
                aSheetPath = sheet;
                return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
            }
        }
    }


    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::CreateBusWireEntry( const VECTOR2I& position, std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_SCREEN*         screen = frame->GetScreen();
    SCH_BUS_WIRE_ENTRY* entry = new SCH_BUS_WIRE_ENTRY{ position };

    entry->SetParent( screen );
    entry->SetFlags( IS_NEW );
    frame->AddToScreen( entry, screen );

    aKID = entry->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::CreateJunction( const VECTOR2I& position, std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC*      schematic = nullptr;
    GetSchEditorFrame( frame );
    GetSchematic( schematic );

    if( nullptr == frame || nullptr == schematic )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SCH_SCREEN* screen = frame->GetScreen();


    if( !screen->IsExplicitJunctionAllowed( position ) )
    {
        frame->ShowInfoBarError( _( "Junction location contains no joinable wires and/or pins." ) );
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_SUPPORTED;
    }

    SCH_JUNCTION* junction = new SCH_JUNCTION( position );
    junction->SetParent( screen );
    // previewItem->SetFlags(IS_NEW);
    frame->AddToScreen( junction, screen );

    aKID = junction->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::GetAllLinesKIID( const SCH_LAYER_ID&       layer,
                                                         std::vector<std::string>& allKIDList )
{
    allKIDList.clear();
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );
    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    for( auto& elem : frame->GetScreen()->Items().OfType( SCH_LINE_T ) )
    {
        if( elem->GetLayer() == layer )
        {
            allKIDList.emplace_back( elem->m_Uuid.AsStdString() );
        }
    }


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::GetLineWidth( const std::string& aKID, double& aLineWidth )
{
    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aLineWidth = sch_line->GetLineWidth();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::SetLineColor( const std::string& aKID, const KIGFX::COLOR4D& aColor )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_line->SetLineColor( aColor );

    frame->UpdateItem( sch_line );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::SetLineWidth( const std::string& aKID, const int& aLineWidth )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_line->SetLineWidth( aLineWidth );

    frame->UpdateItem( sch_line );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::SetLineStart( const std::string& aKID, const VECTOR2I& aStart )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_line->SetStartPoint( aStart );
    frame->UpdateItem( sch_line );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::SetLineEnd( const std::string& aKID, const VECTOR2I& aEnd )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    sch_line->SetEndPoint( aEnd );

    frame->UpdateItem( sch_line );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::MoveLine( const std::string& aKID, const VECTOR2I& aOffset, const int& type )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    SCH_LINE* sch_line = nullptr;
    FindLineByKIID( aKID, sch_line );

    if( nullptr == sch_line )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    if( type == 0 )
    {
        sch_line->Move( aOffset );
    }
    else if( type == 1 )
    {
        sch_line->MoveStart( aOffset );
    }
    else if( type == 2 )
    {
        sch_line->MoveEnd( aOffset );
    }
    else
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    frame->UpdateItem( sch_line );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::GetWireDriver( const std::string& aKID, std::string& aDriverName )
{
    SCH_LINE*      sch_line = nullptr;
    SCH_SHEET_PATH sheet_path;
    FindLineByKIID( aKID, sch_line, sheet_path );

    if( nullptr == sch_line )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    auto item = sch_line->Connection( &sheet_path )->Driver();
    if( item )
    {
        // 暂时返回空字符串，等待具体实现
        aDriverName = "";
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_LINE::GetWireNetName( const std::string& aKID, std::string& aNetName )
{
    SCH_LINE*      sch_line = nullptr;
    SCH_SHEET_PATH sheet_path;
    FindLineByKIID( aKID, sch_line, sheet_path );

    if( nullptr == sch_line )
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;

    aNetName = sch_line->Connection( &sheet_path )->GetNetName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}