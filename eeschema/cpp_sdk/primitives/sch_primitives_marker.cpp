#include "sch_primitives_marker.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_marker.h>

KI_SDK_EXEC_RESULT FindMarkerByKIID( const std::string& aKID, SCH_MARKER*& aMarker)
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_SCREENS screens( frame->Schematic().Root() );
    for( SCH_SCREEN* screen = screens.GetFirst(); screen != nullptr; screen = screens.GetNext())
    {
        for( SCH_ITEM* item : screen->Items().OfType( SCH_MARKER_T ))
        {
            if( item->m_Uuid.AsStdString() == aKID )
            {
                // aMarker = item;
                aMarker = static_cast<SCH_MARKER*>( item );
                return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
            }
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_MARKER::GetAllMarkerKIID( std::vector<std::string>& aKIDList )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_SCREENS screens( frame->Schematic().Root() );

    for( SCH_SCREEN* screen = screens.GetFirst(); screen != nullptr; screen = screens.GetNext() )
    {
        for( SCH_ITEM* item : screen->Items().OfType( SCH_MARKER_T ) )
        {
            SCH_MARKER* marker = static_cast<SCH_MARKER*>( item );
            aKIDList.push_back( marker->m_Uuid.AsStdString() );
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_MARKER::FindMarkerKIDByPosition( const VECTOR2I& aPosition, std::string& aKID )
{
    std::vector<std::string> marker_list;
    GetAllMarkerKIID( marker_list );
    
    if( marker_list.empty() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    for( auto& marker_uuid : marker_list )
    {
        SCH_MARKER* marker = nullptr;
        FindMarkerByKIID( marker_uuid, marker );
        
        if( nullptr == marker )
            continue;

        VECTOR2I pos = marker->GetPosition();
        if( pos.Distance( aPosition ) == 0 )
        {
            aKID = marker_uuid;
            return KI_SDK_EXEC_RESULT::KI_SDK_ALREADY_EXISTS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_MARKER::ClearMarkerAtPosition( const VECTOR2I& aPosition, SCH_SHEET_PATH& aSheetPath )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    std::string        marker_uuid;
    FindMarkerKIDByPosition( aPosition, marker_uuid );

    if( marker_uuid.empty() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SCH_MARKER* marker = nullptr;
    FindMarkerByKIID( marker_uuid, marker );

    if( marker )
    {
        aSheetPath.LastScreen()->Remove( marker );
        frame->GetCanvas()->GetView()->Remove( marker );
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_MARKER::AddMarkerAtPosition( const VECTOR2I& aPosition, SCH_SHEET_PATH& aSheetPath,
                                                                SCH_PIN* aPin,  std::string& aKID )
{
    SCH_EDIT_FRAME* frame = nullptr;
    GetSchEditorFrame( frame );

    if( nullptr == frame )
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    VECTOR2I pin_pos = aPin->GetPosition();
    std::string marker_uuid;
    KI_SDK_EXEC_RESULT status = FindMarkerKIDByPosition( pin_pos, marker_uuid );

    if( KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND == status )
    {
        std::shared_ptr<ERC_ITEM> ercItem = ERC_ITEM::Create( ERCE_T::ERCE_GENERIC_ERROR );
        ercItem->SetSheetSpecificPath( aSheetPath );
        ercItem->SetItemsSheetPaths( aSheetPath );
        ercItem->SetItems( aPin );

        SCH_MARKER* marker = new SCH_MARKER( ercItem, aPin->GetPosition() );
        aSheetPath.LastScreen()->Append( marker );

        frame->GetCanvas()->GetView()->Add( marker );

        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }
    
    return KI_SDK_EXEC_RESULT::KI_SDK_ALREADY_EXISTS;
}