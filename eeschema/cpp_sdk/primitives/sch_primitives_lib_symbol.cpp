#include "sch_primitives_lib_symbol.h"

#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_symbol.h>
#include <io/io_mgr.h>
#include <sch_io/sch_io.h>
#include <math/box2.h>
#include <symbol_lib_table.h>
#include "sch_primitives_line.h"
#include "sch_primitives_label.h"
#include <project_sch.h>
#include <symbol_editor/lib_symbol_library_manager.h>
#include <kiway.h>
#include <mail_type.h>
#include <sch_pin.h>
#include <pin_type.h>


KI_SDK_EXEC_RESULT FindLibSymbolPinByKIID( const std::string& aKID, SCH_PIN*& aPin )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* symbol = frame->GetCurSymbol();

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for( auto& item : symbol->GetDrawItems() )
    {
        if( item.m_Uuid.AsStdString() == aKID && item.Type() == SCH_PIN_T )
        {
            aPin = static_cast<SCH_PIN*>( &item );
            return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::CreateLibSymbolLibrary( const std::string& aLibraryName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    SYMBOL_LIB_TABLE* lib_table = PROJECT_SCH::SchSymbolLibTable( &frame->Prj() );

    // wxFileName fn = frame->GetLibManager().GetUniqueLibraryName();


    wxString lib_name = wxString::FromUTF8( aLibraryName );

    if( frame->GetLibManager().LibraryExists( lib_name ) )
    {
        // DisplayError( this, wxString::Format( _( "Library '%s' already exists." ), libName ) );
        return KI_SDK_EXEC_RESULT::KI_SDK_ALREADY_EXISTS;
    }

    wxFileName lib_path( frame->Prj().GetProjectPath(), lib_name, FILEEXT::KiCadSymbolLibFileExtension );

    frame->GetLibManager().CreateLibrary( lib_path.GetFullPath(), *lib_table );

    frame->SavePrjSymbolLibTables();

    std::string packet = lib_path.GetFullPath().ToStdString();
    frame->Kiway().ExpressMail( FRAME_SCH_SYMBOL_EDITOR, MAIL_LIB_EDIT, packet );

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::CreateEmptyLibSymbol( const std::string& aLibraryName,
                                                                   const std::string& aSymbolName,
                                                                   const std::string& aReference )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );
    wxString msg;

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    wxString lib_name = wxString::FromUTF8( aLibraryName );

    if( !frame->GetLibManager().LibraryExists( lib_name ) )
    {
        msg.Printf( _( "Symbol Library '%s' not found." ), lib_name );
        frame->ShowInfoBarError( msg );
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    if( frame->GetLibManager().IsLibraryReadOnly( lib_name ) )
    {
        msg.Printf( _( "Symol Library '%s' is ReadOnly." ), lib_name );
        frame->ShowInfoBarError( msg );
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    LIB_SYMBOL new_symbol( aSymbolName );

    wxString ref_text = wxString::FromUTF8( aReference );
    new_symbol.GetReferenceField().SetText( ref_text );

    new_symbol.SetUnitCount( 1 );
    new_symbol.SetPinNameOffset( 0 );

    //POWRER or normal
    new_symbol.SetNormal();

    frame->GetLibManager().UpdateSymbol( &new_symbol, lib_name );
    frame->SyncLibraries( false );
    frame->LoadSymbol( aSymbolName, lib_name, 1 );


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::CreateLibSymbolPin( const std::string& aPinName,
                                                                 const std::string& aPinNumber,
                                                                 const VECTOR2I&    aPosition,
                                                                 PIN_ORIENTATION    aOrientation,
                                                                 ELECTRICAL_PINTYPE aElectrial, GRAPHIC_PINSHAPE aShape,
                                                                 int aLength, int aNameTextSize, int aNumberTextSize )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* symbol = frame->GetCurSymbol();

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    SCH_PIN* pin = new SCH_PIN( symbol );
    pin->SetFlags( IS_NEW );
    pin->SetPosition( aPosition );
    wxString pin_name = wxString::FromUTF8( aPinName );
    wxString pin_number = wxString::FromUTF8( aPinNumber );
    pin->SetName( pin_name );
    pin->SetNumber( pin_number );
    pin->SetNameTextSize( SchMilsToIU( aNameTextSize ) );
    pin->SetNumberTextSize( SchMilsToIU( aNumberTextSize ) );

    pin->SetLength( SchMilsToIU( aLength ) );
    pin->SetShape( aShape );
    pin->SetOrientation( aOrientation );
    pin->SetType( aElectrial );

    pin->ClearEditFlags();

    symbol->AddDrawItem( pin );

    // frame->GetCanvas()->GetView()->Add( pin );
    // frame->GetCanvas()->GetView()->Update( symbol );

    UpdateLibSymbol();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetAllLibSymbolPinsKIID( std::vector<std::string>& allKIDList )
{
    allKIDList.clear();
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* symbol = frame->GetCurSymbol();

    if( nullptr == symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_INVALID_INPUT;
    }

    for( auto& item : symbol->GetDrawItems() )
    {
        if( SCH_PIN_T == item.Type() )
        {
            allKIDList.push_back( item.m_Uuid.AsStdString() );
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinName( const std::string& aKID, std::string& aPinName )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPinName = pin->GetName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNumber( const std::string& aKID, std::string& aPinNumber )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPinNumber = pin->GetNumber().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinElectricalType( const std::string&  aKID,
                                                                            ELECTRICAL_PINTYPE& aElectrial )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aElectrial = pin->GetType();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinGraphicStyle( const std::string& aKID,
                                                                          GRAPHIC_PINSHAPE&  aStyle )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aStyle = pin->GetShape();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinPosition( const std::string& aKID, VECTOR2I& aPosition )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aPosition = pin->GetPosition();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinOrientation( const std::string& aKID,
                                                                         PIN_ORIENTATION&   aOrientation )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aOrientation = pin->GetOrientation();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinLength( const std::string& aKID, int& aLength )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aLength = SchIUToMils( pin->GetLength() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNameTextSize( const std::string& aKID, int& aSize )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aSize = SchIUToMils( pin->GetNameTextSize() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNumberTextSize( const std::string& aKID, int& aSize )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    aSize = SchIUToMils( pin->GetNumberTextSize() );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinName( const std::string& aKID, const std::string& aName )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    wxString pin_name = wxString::FromUTF8( aName );
    pin->SetName( pin_name );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNumber( const std::string& aKID,
                                                                    const std::string& aNumber )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    wxString pin_number = wxString::FromUTF8( aNumber );
    pin->SetNumber( pin_number );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinGraphicStyle( const std::string& aKID,
                                                                          GRAPHIC_PINSHAPE   aShape )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetShape( aShape );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinElectricalType( const std::string& aKID,
                                                                         ELECTRICAL_PINTYPE aType )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetType( aType );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinPosition( const std::string& aKID,
                                                                      const VECTOR2I&    aPosition )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetPosition( aPosition );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinOrientation( const std::string& aKID,
                                                                         PIN_ORIENTATION    aOrientation )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetOrientation( aOrientation );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinLength( const std::string& aKID, int mils )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetLength( SchMilsToIU( mils ) );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNameTextSize( const std::string& aKID, int aSize )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetNameTextSize( SchMilsToIU( aSize ) );
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::RotateLibSymbolPin( const std::string& aKID, const bool& aRotateCCW )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->Rotate( pin->GetPosition(), aRotateCCW );
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::HorizontalLibSymbolPin( const std::string& aKID )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin ) 
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->MirrorHorizontally( pin->GetBoundingBox().GetCenter().x);
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::VerticalLibSymbolPin( const std::string& aKID )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin);

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->MirrorVertically( pin->GetBoundingBox().GetCenter().y);
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNumberTextSize( const std::string& aKID, int aSize )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    pin->SetNumberTextSize( SchMilsToIU( aSize ) );
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::RemoveLibSymbolPin( const std::string& aKID )
{
    SCH_PIN* pin = nullptr;
    FindLibSymbolPinByKIID( aKID, pin );

    if( nullptr == pin )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }

    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    frame->GetCurSymbol()->RemoveDrawItem( pin );

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::UpdateLibSymbol( wxString* aOldName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( frame )
    {
        frame->RebuildView();
    }

    if( aOldName )
    {
        frame->UpdateAfterSymbolProperties( aOldName );
    }
    


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}




KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolUnitCount( int& aCount )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aCount =  lib_symbol->GetUnitCount();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolUnit( int& aUnit )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aUnit = lib_symbol->GetUnit();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolUnitName( const int& aUnit, std::string& aUnitName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aUnitName = lib_symbol->GetUnitDisplayName( aUnit, true ).ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolReference( std::string& aReference )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aReference = lib_symbol->GetReferenceField().GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolValue( std::string& aValue )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aValue = lib_symbol->GetValueField().GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolFootprint( std::string& aFootprint )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aFootprint = lib_symbol->GetFootprintField().GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolDataSheet( std::string& aDataSheet )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aDataSheet = lib_symbol->GetDatasheetField().GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolDescription( std::string& aDescription )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aDescription = lib_symbol->GetDescriptionField().GetText().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolUnitCount( const int& aUnitCount )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetUnitCount( aUnitCount );

    wxString old_name = lib_symbol->GetName();
    UpdateLibSymbol( &old_name );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolUnit( const int& aUnit )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol || aUnit > lib_symbol->GetUnitCount() )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetUnit( aUnit );

    // wxString old_name = lib_symbol->GetName();

    frame->SetUnit( aUnit );

    UpdateLibSymbol() ;
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolUnitName( const int& aUnit, const std::string& aUnitName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    wxString unit_name = wxString::FromUTF8( aUnitName );
    lib_symbol->SetUnitDisplayName( aUnit, unit_name );

    wxString old_name = lib_symbol->GetName();
    UpdateLibSymbol( &old_name );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolFieldsMap( std::map<std::string, std::string>& aFieldsMap )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    std::vector<SCH_FIELD*> fields_list;
    lib_symbol->GetFields( fields_list );

    for( auto& field : fields_list )
    {
        std::string field_name = field->GetName().ToStdString();
        std::string field_text = field->GetText().ToStdString();
        if( aFieldsMap.find( field_name ) == aFieldsMap.end() )
        {
            aFieldsMap.insert( std::make_pair( field_name, field_text ) );
        }
    }
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::ShowLibSymbolFieldByName( const std::string& aFieldName,
                                                                       const bool&        isShow,
                                                                       const bool&        isShowName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    wxString field_name = wxString::FromUTF8( aFieldName );
    if( SCH_FIELD* sch_field = lib_symbol->FindField( field_name ) )
    {
        sch_field->SetShowInChooser( isShow );
        sch_field->SetNameShown( isShowName );

        UpdateLibSymbol();
        return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::AddLibSymbolField( const std::string& aFieldName,
                                                                const std::string& aFieldText )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    std::map<std::string, std::string> field_list;
    GetLibSymbolFieldsMap( field_list );

    wxString field_name = wxString::FromUTF8( aFieldName );
    wxString field_text = wxString::FromUTF8( aFieldText );
    SCH_FIELD* field = lib_symbol->FindField( field_name );

    if( nullptr == field )
    {
        field = new SCH_FIELD( lib_symbol->GetPosition(), LAYER_FIELDS, lib_symbol, field_name );
        field->SetText( field_text );
        field->SetVisible( false );
        lib_symbol->AddField( *field );
    }else
    {
        field->SetText( field_text );
    }

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::IsLibSymbolPower( bool& aIsPower)
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aIsPower = lib_symbol->IsPower();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPower( const bool& aPower )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    if( aPower )
    {
        lib_symbol->SetPower();
    }else
    {
        lib_symbol->SetNormal();
    }
    

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolExcludeFromBoard( bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aExclude = lib_symbol->GetExcludedFromBoard();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolExcludeFromSimulation( bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr ==  frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aExclude = lib_symbol->GetExcludedFromSim();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolExcludeFromBom( bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aExclude = lib_symbol->GetExcludedFromBOM();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolExcludeFromBoard( const bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetExcludedFromBoard( aExclude );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolExcludeFromSimulation( const bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetExcludedFromSim( aExclude );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolExcludeFromBom( const bool& aExclude )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetExcludedFromBOM( aExclude );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNameOffset( int& mils )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    int offset = lib_symbol->GetPinNameOffset();
    mils = SchIUToMils( offset );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNameOffset( const int& mils )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    
    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();
    
    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    int offset = SchMilsToIU( mils );
    lib_symbol->SetPinNameOffset( offset );
    
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolName( std::string& aName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aName = lib_symbol->GetName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolLibraryName( std::string& aLibraryName )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aLibraryName = lib_symbol->GetLibraryName().ToStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNumbersIsShow( bool& isShow )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    isShow = lib_symbol->GetShowPinNumbers();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolPinNamesIsShow( bool& isShow )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    isShow = lib_symbol->GetShowPinNames();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinInside( const bool& aInside )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    if( aInside )
    {
        int mils;
        GetLibSymbolPinNameOffset( mils );

        // We interpret an offset of 0 as "outside", so make sure it's non-zero
        lib_symbol->SetPinNameOffset( mils == 0 ? 20 : SchMilsToIU(mils) );
    }
    else
    {
        lib_symbol->SetPinNameOffset( 0 ); // pin text outside the body (name is on the pin)
    }

    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNumbersShow( const bool& aShow )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetShowPinNumbers( aShow );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolPinNamesShow( const bool& aShow )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetShowPinNames( aShow );
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::GetLibSymbolAlternateBodyStyle( bool& aStyle )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aStyle = lib_symbol->HasAlternateBodyStyle();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMTIVES_LIB_SYMBOL::SetLibSymbolAlternateBodyStyle( const bool& aStyle )
{
    SYMBOL_EDIT_FRAME* frame = nullptr;
    GetSymbolFrame( frame );

    if( nullptr == frame )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_BOARD_ERROR;
    }

    LIB_SYMBOL* lib_symbol = frame->GetCurSymbol();

    if( nullptr == lib_symbol )
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    lib_symbol->SetHasAlternateBodyStyle( aStyle );
    UpdateLibSymbol();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}
