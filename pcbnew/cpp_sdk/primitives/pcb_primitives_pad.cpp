#include <vector>
#include <string>
#include <wx/string.h>
#include <pad.h>
#include <padstack.h>
#include <footprint.h>
#include <footprint_edit_frame.h>
#include <board_design_settings.h>


#include "pcb_primitives_pad.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>


PADSTACK_MODE ConvertPadstackMode( PADSTACK::MODE internalMode )
{
    switch( internalMode )
    {
    case PADSTACK::MODE::NORMAL: return PADSTACK_MODE::NORMAL;

    case PADSTACK::MODE::FRONT_INNER_BACK: return PADSTACK_MODE::FRONT_INNER_BACK;

    case PADSTACK::MODE::CUSTOM: return PADSTACK_MODE::CUSTOM;

    default: throw std::invalid_argument( "Unknown PADSTACK::MODE value" );
    }
}


PADSTACK::MODE ConvertToInternalMode( PADSTACK_MODE externalMode )
{
    switch( externalMode )
    {
    case PADSTACK_MODE::NORMAL: return PADSTACK::MODE::NORMAL;

    case PADSTACK_MODE::FRONT_INNER_BACK: return PADSTACK::MODE::FRONT_INNER_BACK;

    case PADSTACK_MODE::CUSTOM: return PADSTACK::MODE::CUSTOM;

    default: throw std::invalid_argument( "Unknown PADSTACK_MODE value" );
    }
}


KI_SDK_EXEC_RESULT FindPadByKIID( const std::string& aKID, PAD*& aPad )
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );
    
    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }

    FOOTPRINT* footprint = frame->GetBoard()->GetFirstFootprint();

    if( nullptr == footprint )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for( auto& pad : footprint->Pads())
    {
        if(pad->m_Uuid.AsStdString() == aKID)
        {
            aPad = pad;
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT UpdateFootprintEditFrame( PAD* pad = nullptr)
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    FOOTPRINT* footprint = frame->GetBoard()->GetFirstFootprint();

    if( footprint )
    {

        if( pad )
        {
            footprint->Add( pad );
            frame->GetCanvas()->GetView()->Add( pad );
        }

        frame->GetCanvas()->GetView()->Update( footprint );

    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::InitFootprintFrameAndBoard()
{
    GetFootprintEditorFrame( m_footprintframe );

    if( m_footprintframe )
    {
        m_board = m_footprintframe->GetBoard();
    }else
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

PCB_PRIMITIVES_PAD::PCB_PRIMITIVES_PAD() : 
                    m_footprintframe(nullptr),
                    m_board(nullptr)
{
    InitFootprintFrameAndBoard();
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::PlaceCustomPad( const PAD_ATTRIB& aPadAttrib, const std::string& aNumber,
                                                       const VECTOR2I& aPosition, const PAD_SHAPE& aPadShape,
                                                       const VECTOR2I& aPadSize, const PAD_DRILL_SHAPE& aDrillShape,
                                                       const VECTOR2I& aDrillSize, std::string& aKID )
{
    FOOTPRINT* footprint = m_board->GetFirstFootprint();

    if( nullptr == footprint)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    PAD* pad = new PAD( footprint );
    PAD* master = m_footprintframe->GetDesignSettings().m_Pad_Master.get();

    pad->ImportSettingsFrom( *master );

    pad->SetAttribute( aPadAttrib );
    pad->SetShape( PADSTACK::ALL_LAYERS, aPadShape );
    pad->SetSize( PADSTACK::ALL_LAYERS, aPadSize );

    if( PAD_ATTRIB::SMD == aPadAttrib  )
    {
        pad->SetLayerSet( PAD::SMDMask() );
    }
    else if( PAD_ATTRIB::PTH == aPadAttrib)
    {
        pad->SetLayerSet( PAD::PTHMask() );
    }else if( PAD_ATTRIB::NPTH == aPadAttrib )
    {
        pad->SetLayerSet( LSET::ExternalCuMask() | LSET( { F_Mask, B_Mask } ) );
    }else if (PAD_ATTRIB::CONN == aPadAttrib )
    {
        pad->SetLayerSet(  LSET( { F_Cu, F_Mask } ) );
    }

    pad->SetDrillShape( aDrillShape );
    pad->SetDrillSize( aDrillSize );

    pad->SetNumber( aNumber );
    pad->SetPosition( aPosition );
    pad->SetParent( footprint );
    
    UpdateFootprintEditFrame( pad );
    // footprint->Add( pad );
    // frame->GetCanvas()->GetView()->Add( pad );
    // frame->GetCanvas()->GetView()->Update( footprint );

    aKID = pad->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::PlacePad( const std::string& aNumber, const VECTOR2I& aPosition, std::string& aKID )
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );

    if( frame == nullptr )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD* board = frame->GetBoard();

    if( nullptr == board )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    FOOTPRINT* footprint = board->GetFirstFootprint();

    if( nullptr == footprint )
    {
        wxLogError( "Please Select a footprint int the Left Panel." );
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    PAD* pad = new PAD( footprint );
    PAD* master = frame->GetDesignSettings().m_Pad_Master.get();

    pad->ImportSettingsFrom( *master );

    if( ( board->GetFirstFootprint()->GetAttributes() & FP_SMD ) && master->GetAttribute() == PAD_ATTRIB::PTH )
    {
        if( pad->GetProperty() != PAD_PROP::HEATSINK )
        {
            pad->SetAttribute( PAD_ATTRIB::SMD );
            pad->SetShape( PADSTACK::ALL_LAYERS, PAD_SHAPE::ROUNDRECT );
            pad->SetSizeX( 1.5 * pad->GetSizeY() );
            pad->SetLayerSet( PAD::SMDMask() );
        }
    }
    else if( ( board->GetFirstFootprint()->GetAttributes() & FP_THROUGH_HOLE )
             && master->GetAttribute() == PAD_ATTRIB::SMD )
    {
        pad->SetAttribute( PAD_ATTRIB::PTH );
        pad->SetShape( PADSTACK::ALL_LAYERS, PAD_SHAPE::CIRCLE );
        pad->SetSize( PADSTACK::ALL_LAYERS, VECTOR2I( pad->GetSizeX(), pad->GetSizeX() ) );

        pad->SetDrillShape( PAD_DRILL_SHAPE::CIRCLE );
        int hole_size = pad->GetSizeX() / 2;
        pad->SetDrillSize( VECTOR2I( hole_size, hole_size ) );

        pad->SetLayerSet( PAD::PTHMask() );
    }

    if( aNumber.empty())
    {
        if( pad->CanHaveNumber() )
        {
            wxString padNumber;

            padNumber = board->GetFirstFootprint()->GetNextPadNumber( padNumber );

            pad->SetNumber( padNumber );
        }
    }else
    {
        pad->SetNumber( wxString::FromUTF8(aNumber) );
    }
       

    pad->SetPosition( aPosition );
    pad->SetParent( footprint );
    footprint->Add( pad );

    frame->GetCanvas()->GetView()->Add( pad );
    frame->GetCanvas()->GetView()->Update( footprint );

    aKID = pad->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::PlacePad_old( std::string& aKID )
{
    FOOTPRINT_EDIT_FRAME* frame = nullptr;
    GetFootprintEditorFrame( frame );

    if( frame == nullptr )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD* board = frame->GetBoard();

    if( nullptr == board )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    FOOTPRINT* footprint = board->GetFirstFootprint();

    if( nullptr == footprint )
    {
        wxLogError( "Please Select a Footprint." );
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    PAD* pad = new PAD( footprint );
    PAD* master = frame->GetDesignSettings().m_Pad_Master.get();

    pad->ImportSettingsFrom( *master );

    if( ( board->GetFirstFootprint()->GetAttributes() & FP_SMD ) && master->GetAttribute() == PAD_ATTRIB::PTH )
    {
        if( pad->GetProperty() != PAD_PROP::HEATSINK )
        {
            pad->SetAttribute( PAD_ATTRIB::SMD );
            pad->SetShape( PADSTACK::ALL_LAYERS, PAD_SHAPE::ROUNDRECT );
            pad->SetSizeX( 1.5 * pad->GetSizeY() );
            pad->SetLayerSet( PAD::SMDMask() );
        }
    }
    else if( ( board->GetFirstFootprint()->GetAttributes() & FP_THROUGH_HOLE )
             && master->GetAttribute() == PAD_ATTRIB::SMD )
    {
        pad->SetAttribute( PAD_ATTRIB::PTH );
        pad->SetShape( PADSTACK::ALL_LAYERS, PAD_SHAPE::CIRCLE );
        pad->SetSize( PADSTACK::ALL_LAYERS, VECTOR2I( pad->GetSizeX(), pad->GetSizeX() ) );

        // Gives an acceptable drill size: it cannot be 0, but from pad master
        // it is currently 0, therefore change it:
        pad->SetDrillShape( PAD_DRILL_SHAPE::CIRCLE );
        int hole_size = pad->GetSizeX() / 2;
        pad->SetDrillSize( VECTOR2I( hole_size, hole_size ) );

        pad->SetLayerSet( PAD::PTHMask() );
    }

    if( pad->CanHaveNumber() )
    {
        wxString padNumber;

        // Use the last entered pad number when recreating a pad without using the
        // previously created pad, and a new number when creating a really new pad
        padNumber = board->GetFirstFootprint()->GetNextPadNumber( padNumber );

        pad->SetNumber( padNumber );
        // If a pad is recreated and the previously created was not placed, use
        // the last entered pad number
    }
    pad->SetPosition( VECTOR2I( 0, 0 ) );
    pad->SetParent( footprint );

    footprint->Add( pad );

    frame->GetCanvas()->GetView()->Add( pad );
    frame->GetCanvas()->GetView()->Update( footprint );

    aKID = pad->m_Uuid.AsStdString();


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadPosition( const std::string& aKID, VECTOR2I& position )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    position = pad->GetPosition();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadNumber( const std::string& aKID, std::string& number )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    number = pad->GetNumber();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadNetName( const std::string& aKID, std::string& netName )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    netName = pad->GetNetname();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadNetCode( const std::string& aKID, int& netCode )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    netCode = pad->GetNetCode();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadIsLocked( const std::string& aKID, bool& isLocked )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    isLocked = pad->IsLocked();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadAttribute( const std::string& aKID, PAD_ATTRIB& attribute )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    attribute = pad->GetAttribute();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadToDieLength( const std::string& aKID, double& distance )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    distance = pad->GetPadToDieLength();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadLocalClearance( const std::string& aKID, double& clearance )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    std::optional<int> opt = pad->GetLocalClearance();
    if( opt.has_value() )
    {
        clearance = opt.value();
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }
    else
    {
        clearance = -1;
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackMode( const std::string& aKID, PADSTACK_MODE& mode )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    mode = ConvertPadstackMode( pad->Padstack().Mode() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackDegree( const std::string& aKID, double& degree )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    degree = pad->Padstack().GetOrientation().AsDegrees();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackDrillStartLayer( const std::string& aKID, PCB_LAYER_ID& startLayer )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    startLayer = pad->Padstack().StartLayer();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackDrillEndLayer( const std::string& aKID, PCB_LAYER_ID& endLayer )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    endLayer = pad->Padstack().EndLayer();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackDrillSize( const std::string& aKID, VECTOR2I& size )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    size = VECTOR2I( pad->GetDrillSizeX(), pad->GetDrillSizeY() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadStackSize( const std::string& aKID, const PCB_LAYER_ID& layer,
                                                        VECTOR2I& size )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    size = pad->GetSize( layer );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadPosition( const std::string& aKID, const VECTOR2I& position )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetPosition( position );

    UpdateFootprintEditFrame();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadKIDByNumber( const std::string& aNumber, std::vector<std::string>& aKIDList)
{
    aKIDList.clear();
    
    std::vector<std::string> pad_list;
    GetAllPadsKIID(pad_list);
    for( auto& pad_uuid : pad_list)
    {
        std::string number;
        GetPadNumber( pad_uuid, number );
        if( number == aNumber)
        {
            aKIDList.push_back( pad_uuid );
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::MovePad( const std::string& aKID, const VECTOR2I& offset )
{
    PAD* pad = nullptr;
    FindPadByKIID( aKID, pad );

    if( nullptr == pad )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pad->Move( offset );

    UpdateFootprintEditFrame();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::RotatePad( const std::string& aKID, double degree )
{
    PAD* pad = nullptr;
    FindPadByKIID( aKID, pad );

    if( nullptr == pad )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    EDA_ANGLE eda_angle( degree, DEGREES_T );
    pad->Rotate( pad->GetCenter(), eda_angle );

    UpdateFootprintEditFrame();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::DeletePadByKIID( const std::string& aKID )
{
    // FOOTPRINT_EDIT_FRAME* frame = nullptr;
    // GetFootprintEditorFrame( frame );

    // if( nullptr == frame )
    // {
    //     return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    // }

    FOOTPRINT* footprint = m_board->GetFirstFootprint();

    PAD* pad = nullptr;
    FindPadByKIID( aKID, pad );

    if( nullptr == pad )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    footprint->Remove( pad );
    
    m_footprintframe->GetCanvas()->GetView()->Remove( pad );

    m_footprintframe->GetCanvas()->GetView()->Update( footprint );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadNumber( const std::string& aKID, const std::string& number )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetNumber( number );

    UpdateFootprintEditFrame();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadNetCode( const std::string& aKID, int netCode )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetNetCode( netCode );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadAttribute( const std::string& aKID, PAD_ATTRIB attribute )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetAttribute( attribute );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadIsLocked( const std::string& aKID, bool isLocked )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetLocked( isLocked );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadToDieLength( const std::string& aKID, double distance )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetPadToDieLength( distance );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadLocalClearance( const std::string& aKID, double clearance )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetLocalClearance( clearance );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackMode( const std::string& aKID, PADSTACK_MODE mode )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->Padstack().SetMode( ConvertToInternalMode( mode ) );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackSize( const std::string& aKID,
                                                        VECTOR2I           size )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->SetSizeX( size.x );
    pad->SetSizeY( size.y );

    UpdateFootprintEditFrame();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackDegree( const std::string& aKID, double degree )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->Padstack().SetOrientation( EDA_ANGLE( degree, DEGREES_T ) );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackDrillStartLayer( const std::string& aKID, PCB_LAYER_ID startLayer )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->Padstack().Drill().start = startLayer;
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackDrillEndLayer( const std::string& aKID, PCB_LAYER_ID endLayer )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );
    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }
    pad->Padstack().Drill().end = endLayer;
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadStackDrillSize( const std::string& aKID, VECTOR2I size )
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }

    pad->SetDrillSize( size );

    UpdateFootprintEditFrame();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::SetPadDrillShape( const std::string& aKID, const PAD_DRILL_SHAPE& aShape)
{
    PAD*               pad = nullptr;
    KI_SDK_EXEC_RESULT status = FindPadByKIID( aKID, pad );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !pad )
    {
        return status;
    }

    pad->SetDrillShape( aShape );

    UpdateFootprintEditFrame();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetAllPadsKIID( std::vector<std::string>& allKIDList )
{
    // allKIDList.clear();
    // FOOTPRINT_EDIT_FRAME* frame = nullptr;
    // GetFootprintEditorFrame( frame );

    // if( nullptr == frame )
    // {
    //     return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    // }

    FOOTPRINT* footprint = m_footprintframe->GetBoard()->GetFirstFootprint();
    if( nullptr == footprint )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    for(auto& pad : footprint->Pads())
    {
        allKIDList.push_back(pad->m_Uuid.AsStdString());
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_PAD::GetPadLayers( const std::string& aKID, std::vector<PCB_LAYER_ID>& aLayerList )
{
    PAD* pad = nullptr;
    FindPadByKIID( aKID, pad );

    if( nullptr == pad )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aLayerList = pad->GetLayerSet().Seq();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}