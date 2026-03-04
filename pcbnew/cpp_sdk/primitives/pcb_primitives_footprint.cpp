#include <board_item.h>
#include <math/vector2d.h>
#include <footprint.h>
#include <pad.h>
#include <pcb_edit_frame.h>
#include <pcbnew_settings.h>
#include <wx/app.h>
#include <board.h>
#include <kiid.h>
#include <pcb_field.h>
#include <pad.h>
#include <vector>
#include <string>
#include <project_pcb.h>
#include <view/view_controls.h>

#include "pcb_primitives_footprint.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>

KI_SDK_EXEC_RESULT  FindFootprintByReference(const wxString& ref, FOOTPRINT*& footprint)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS !=  status || !board )
    {
        return status;
    }
    for(FOOTPRINT* fp : board->Footprints())
    {
        if(fp->GetReference().CmpNoCase(ref) == 0)
        {
            footprint = fp;
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}

KI_SDK_EXEC_RESULT FindFootprintByKIID(const std::string& aKID, FOOTPRINT*& footprint)
{
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(status != KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS || !board)
    {
        return status;
    }
    KIID  aID = KIID(aKID);
    for( FOOTPRINT* fp : board->Footprints() )
    {
        if( fp->m_Uuid == aID )
        {
            footprint = fp;
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
            
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_FOOTPRINT::PlaceFootprint(const std::string& libName, const std::string& fpName,const VECTOR2I& position, EDA_ANGLE& angle, std::string& aKID)
{
    PcbUtilsContext context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status ||  !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;
    BOARD* board = context.board;

    LIB_ID fpid;
    FOOTPRINT*  footprint = nullptr;
    wxString footprintName = wxString::FromUTF8(libName + ":" + fpName);
    int offset = fpid.Parse(UTF8(footprintName));
    if(offset != -1)
    {
        wxLogError("Failed to parse footprint name: %s", footprintName);
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    try
    {
        footprint = frame->LoadFootprint(fpid);
    }
    catch(const std::exception& e)
    {
        wxLogError("Failed to load footprint %s, %s", footprintName, e.what());
        footprint = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    if( nullptr == footprint )
    {
        wxLogError( "Can not Load Footprint : %s ", fpid.Format().c_str() );
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    footprint->SetLink(niluuid);
    footprint->SetFlags(IS_NEW);
    footprint->SetParent(board);
    
    board->UpdateUserUnits( footprint , frame->GetCanvas()->GetView());
    
    for( PAD* pad : footprint->Pads() )
    {
        pad->SetLocalRatsnestVisible( frame->GetPcbNewSettings()->m_Display.m_ShowGlobalRatsnest );

        pad->SetNetCode( 0 );
    }

    footprint->SetOrientation( ANGLE_0 );
    footprint->SetPosition(position);

    board->Add(footprint);
    frame->GetCanvas()->GetView()->Add( footprint );

    aKID = footprint->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_FOOTPRINT::GetAllFootprintKIID(std::vector<std::string>&  allKID)
{
    allKID.clear();
    BOARD* board = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbBoard(board);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !board)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_BOARD_ERROR;
    }
    for(FOOTPRINT*  footprint : board->Footprints())
    {
        allKID.push_back(footprint->m_Uuid.AsStdString());
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_FOOTPRINT::GetFootprintPosition(const std::string& aKID, VECTOR2I& position)
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(status != KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS || !footprint)
    {
        return status;
    }
    
    position = footprint->GetPosition();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetFootprintReference( const std::string& aKID, std::string& reference )
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status  =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status  || !footprint)
    {
        return status;
    }
    reference = footprint->GetReferenceAsString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetFootprintFPID( const std::string& aKID, std::string& fPID )
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    fPID = footprint->GetFPID().Format().wx_str();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetKIIDByReference( const std::string& reference, std::string& aKID )
{
    if(reference.find("REF*")  != std::string::npos || reference.find("ref*") != std::string::npos)
    {
        aKID = "";
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    FOOTPRINT*  footprint = nullptr;
    KI_SDK_EXEC_RESULT status = FindFootprintByReference(reference, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        aKID = "";
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    aKID = footprint->m_Uuid.AsStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetFootprintValue( const std::string& aKID, std::string& value )
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    value = footprint->GetValueAsString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetFootprintFieldText( const std::string& aKID,
                                                                    const std::string& aFieldName, std::string& text )
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    // FIXME
    // text = footprint->GetFieldText(aFieldName);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::MoveFootprintByKIID( const std::string& aKID, const VECTOR2I& offset )
{
    PcbUtilsContext    context;
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    FOOTPRINT* footprint = nullptr;
    status = FindFootprintByKIID( aKID, footprint );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint )
    {
        return status;
    }

    PCB_EDIT_FRAME* frame = context.frame;

    footprint->Move( offset );

    frame->GetCanvas()->GetView()->Update( footprint );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::SetFootprintPosition(const std::string& aKID, VECTOR2I position)
{
    PcbUtilsContext context;
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    FOOTPRINT* footprint  = nullptr;
    status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }

    PCB_EDIT_FRAME* frame = context.frame;

    footprint->SetPosition(position);

    frame->GetCanvas()->GetView()->Update(footprint);
    frame->GetCanvas()->Refresh();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::RotateFootprintByKIID( const std::string& aKID, double degree )
{
    PcbUtilsContext    context;
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;

    FOOTPRINT* footprint = nullptr;
    status = FindFootprintByKIID( aKID, footprint );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint )
    {
        return status;
    }
    EDA_ANGLE eda_angle( degree, DEGREES_T );
    footprint->Rotate( footprint->GetCenter(), eda_angle );
    frame->GetCanvas()->GetView()->Update( footprint );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::SetFootprintReference( const std::string& aKID,
                                                                    const std::string& reference )
{
    PcbUtilsContext    context;
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;

    FOOTPRINT* footprint  = nullptr;
    status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    footprint->SetReference(reference);
    frame->GetCanvas()->GetView()->Update( footprint );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::SetFootprintValue( const std::string& aKID, const std::string& value )
{
    FOOTPRINT*  footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    footprint->SetValue(value);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::AddFootprintField( const std::string& aKID, const std::string& aName,
                                                                const std::string& text )
{
    FOOTPRINT*  footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    if(aName.empty())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    // FIXME
    // PCB_FIELD field = PCB_FIELD(footprint, footprint->GetNextFieldId(), aName);
    // field.SetText(text);
    // footprint->AddField( field );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::SetFootprintFieldText( const std::string& aKID,
                                                                    const std::string& aFieldName,
                                                                    const std::string& text )
{
    FOOTPRINT*  footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    if(aFieldName.empty())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }
    // FIXME
    // PCB_FIELD* field = footprint->GetFieldByName(aFieldName);
    // if(!field)
    // {
    //     return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    // }
    // field->SetText(text);
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_FOOTPRINT::GetFootprintAllPads(const std::string& aKID, std::vector<std::string>& padList)
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT status =  FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    footprint->Pads();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}


KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_FOOTPRINT::GetFootprintLayerID(const std::string& aKID, PCB_LAYER_ID&  layer)
{
    FOOTPRINT* footprint = nullptr;
    KI_SDK_EXEC_RESULT  status = FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    layer = footprint->GetLayer();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}



KI_SDK_EXEC_RESULT   PCB_PRIMITIVES_FOOTPRINT::FlipFootprint(const std::string& aKID, const VECTOR2I& aCentre, FLIP_DIRECTION& aFlipDirection  )
{
    PcbUtilsContext context;
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext(&context);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid())
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    PCB_EDIT_FRAME* frame = context.frame;
    FOOTPRINT*  footprint = nullptr;
    status = FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    footprint->Flip(aCentre, aFlipDirection );

    frame->GetCanvas()->GetView()->Update( footprint );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  PCB_PRIMITIVES_FOOTPRINT::SetFootprintOrientation(const std::string& aKID,  double angle)
{
    PCB_EDIT_FRAME* frame = nullptr;
    KI_SDK_EXEC_RESULT status = GetPcbEditFrame(frame);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !frame)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    FOOTPRINT*  footprint = nullptr;
    status = FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    // if(footprint->GetLayer() == B_Cu)
    // {
    //     angle = angle + 180;
    // }
    EDA_ANGLE orientation( angle, DEGREES_T );
    footprint->SetOrientation(orientation);

    frame->GetCanvas()->GetView()->Update( footprint );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::GetAllPadKIID(const std::string& aKID, std::vector<std::string>& padList)
{
    padList.clear();
    FOOTPRINT*  footprint = nullptr;
    KI_SDK_EXEC_RESULT status = FindFootprintByKIID(aKID, footprint);
    if(KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !footprint)
    {
        return status;
    }
    for( PAD* pad : footprint->Pads() )
    {
        padList.push_back(pad->m_Uuid.AsStdString());
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::CreateNewFootprintInProject( const int& attrib, const std::string& aLibraryName,
                                                                          const std::string& aFootprintName )
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    // const LIB_ID   selected = frame->GetTargetFPID();
    wxString library_name = wxString::FromUTF8(aLibraryName);
    wxString footprint_name = wxString::FromUTF8( aFootprintName );
    //selected.GetUniStringLibNickname();
    FOOTPRINT* newFootprint = frame->CreateNewFootprint( footprint_name, library_name );

    if( !newFootprint )
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;

    if( !frame->Clear_Pcb( true ) )
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;

    frame->GetCanvas()->GetViewControls()->SetCrossHairCursorPosition( VECTOR2D( 0, 0 ), false );
    frame->AddFootprintToBoard( newFootprint );

    // Initialize data relative to nets and netclasses (for a new footprint the defaults are
    // used).  This is mandatory to handle and draw pads.
    frame->GetBoard()->BuildListOfNets();
    newFootprint->SetPosition( VECTOR2I( 0, 0 ) );
    newFootprint->ClearFlags();
    // newFootprint->GetAttributes();
    newFootprint->SetAttributes( attrib );

    frame->Zoom_Automatique( false );
    frame->GetScreen()->SetContentModified();

    // tryToSaveFootprintInLibrary( *newFootprint, selected );

    // FIXME
    // if( !aLibraryName.empty())
    // {
    //     FP_LIB_TABLE& libTable = *PROJECT_PCB::PcbFootprintLibs( &frame->Prj() );

    //     if( !libTable.IsFootprintLibWritable( library_name ) )
    //     {
    //         // If the library is not writeable, we'll give the user a
    //         // footprint not in a library. But add a warning to let them know
    //         // they didn't quite get what they wanted.
    //         frame->ShowInfoBarWarning(
    //                 wxString::Format( _( "The footprint could not be added to the selected library ('%s'). "
    //                                      "This library is read-only." ),
    //                                   library_name ),
    //                 false );
    //         // And the footprint will need to be saved manually
    //     }
    //     else
    //     {
    //         // Go ahead and save it to the library
    //         LIB_ID fpid = newFootprint->GetFPID();
    //         fpid.SetLibNickname( aLibraryName );
    //         newFootprint->SetFPID( fpid );
    //         frame->SaveFootprint( newFootprint );
    //         frame->ClearModify();
    //     }
    //     //
    // }
   

    frame->UpdateView();
    frame->GetCanvas()->ForceRefresh();
    frame->Update3DView( true, true );

    frame->SyncLibraryTree( false );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::CreateFootprintLibraryInProject( const std::string& aLibrary )
{
    PCB_EDIT_FRAME*      pcb_frame = nullptr;
    GetPcbEditFrame( pcb_frame );

    FOOTPRINT_EDIT_FRAME* fp_frame = nullptr;
    GetFootprintEditorFrame( fp_frame );

    PCB_BASE_EDIT_FRAME* frame;

    if( fp_frame )
    {
        frame = fp_frame;
    }
    else if( pcb_frame )
    {
        frame = pcb_frame;
    }else
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    wxString initial_path =  frame->Prj().GetProjectPath();
    wxString   library_name = wxString::FromUTF8( aLibrary + "." + FILEEXT::KiCadFootprintLibPathExtension );
    wxFileName library_path = library_name;

    if(!library_path.IsAbsolute())
    {
        library_path.MakeAbsolute( initial_path );
    }

    // FIXME
    // auto fp_table = PROJECT_PCB::PcbFootprintLibs( &frame->Prj() );
    // frame->CreateNewLibrary( library_path.GetFullPath() );
    // frame->AddLibrary( library_path.GetFullPath(), fp_table );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_FOOTPRINT::IsFootprintLibraryExists( const std::string& aLibraryName, bool& isExists, bool isGlobal )
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    FP_LIB_TABLE* libTable = nullptr;
    // FIXME
    // if( isGlobal )
    // {
    //     libTable = &GFootprintTable;
    // }else
    // {
    //     libTable = PROJECT_PCB::PcbFootprintLibs( &frame->Prj() );
        
    // }
    
    // if(libTable != nullptr)
    // {
    //     isExists = libTable->HasLibrary( aLibraryName );
    //     return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    // }

    return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}