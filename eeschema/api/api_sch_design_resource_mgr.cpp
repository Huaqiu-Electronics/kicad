/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2025 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include <memory>
#include <sch_base_frame.h>
#include <sch_io/sch_io.h>
#include <template_fieldnames.h>
#include <libraries/symbol_library_adapter.h>
#include <api/api_fmt_fp_model_path.h>
#include <api/component_field_name.h>
#include <lib_symbol.h>
#include <sch_field.h>
#include <api/api_design_content.h>
#include <string>
#include <eda_base_frame.h>
#include <project_sch.h>
#include <vector>
#include <wx/log.h>
#include <symbol_library_manager.h>
#include "api_sch_design_resource_mgr.h"
#include "api/api_design_resource_mgr.h"
#include "kiway.h"
#include "lib_id.h"
#include "mail_type.h"
#include "symbol_library_manager.h"


class FIELD_WRITER
{
    LIB_SYMBOL*             m_lib;
    std::vector<SCH_FIELD>& m_fields;

public:
    FIELD_WRITER( LIB_SYMBOL* aLibSym, std::vector<SCH_FIELD>& aFields ) :
            m_lib( aLibSym ),
            m_fields( aFields )
    {
    }

    auto add_field( SCH_FIELD&& field, const wxString& aText )
    {
        field.SetShowInChooser( false );
        field.SetVisible( false );
        field.SetText( aText );
        m_fields.push_back( std::move( field ) );
    }

    auto add_field( const SCH_FIELD& other, const wxString& aText ) { add_field( SCH_FIELD( other ), aText ); }

    auto add_field( const std::string& aName, const std::string& aText )
    {
        add_field( SCH_FIELD( m_lib, {}, aName ), aText );
    };
};


API_SCH_DESIGN_RESOURCE_MGR::API_SCH_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) :
        API_DESIGN_RESOURCE_MGR( aFrame ),
        m_schIo( std::unique_ptr<SCH_IO>( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_KICAD ) ) ),
        m_libMgr( new SYMBOL_LIBRARY_MANAGER( static_cast<SCH_BASE_FRAME&>( aFrame ) ) )
{
    assert( m_schIo );
}

API_SCH_DESIGN_RESOURCE_MGR::~API_SCH_DESIGN_RESOURCE_MGR()
{
}


LIB_SYMBOL* API_SCH_DESIGN_RESOURCE_MGR::SaveComponentToProject( COMPONENT_CONTENT const& aComponent )
{
    const auto symbol_file_name = aComponent.symbol.filename;

    if( !SaveSymbolContentToProject( aComponent.symbol ) )
    {
        return {};
    }

    for( const auto& fpc : aComponent.footprints )
    {
        if( !SaveFootprintContentToProject( fpc.footprint ) )
        {
            wxLogError( "Failed to save footprint %s to project", fpc.footprint.filename );
            continue;
        }

        const auto fp_path = *GetProjectResourceSavePath( PRETTY_DIR, fpc.footprint.filename );

        for( const auto& model : fpc.models )
        {
            Save3dModelToProject( model, fp_path );
        }
    }


    const auto symbol_nick_name = RemoveFileExt( symbol_file_name );
    const auto do_AddLibrary = [this]( wxString aLibFile, LIB_ID id ) -> LIB_SYMBOL*
    {
        wxFileName fn = wxFileName( aLibFile );
        wxString   libName = fn.GetName();

        if( libName.IsEmpty() )
            return {};

        LIBRARY_MANAGER&        manager = Pgm().GetLibraryManager();
        SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &m_frame.Prj() );


        if( !m_libMgr->AddLibrary( fn.GetFullPath(), LIBRARY_TABLE_SCOPE::PROJECT ) )
        {
            return {};
        }

        std::optional<LIBRARY_TABLE*> optTable =
                manager.Table( LIBRARY_TABLE_TYPE::SYMBOL, LIBRARY_TABLE_SCOPE::PROJECT );
        wxCHECK( optTable.has_value(), nullptr );
        LIBRARY_TABLE* table = optTable.value();
        bool           success = true;

        table->Save().map_error(
                [&]( const LIBRARY_ERROR& aError )
                {
                    wxLogError( aError.message );
                    success = false;
                } );

        if( !success )
        {
            return {};
        }

        // FIXME The newly loaded symbol will make the LIB_DATA of existing ones @LIBRARY_MANAGER_ADAPTER::m_libraries messy, so we reload all the project libraries
        adapter->ProjectChanged();
        adapter->AsyncLoad();
        adapter->BlockUntilLoaded();

        std::string payload = "";
        auto        aKiway = &m_frame.Kiway();
        aKiway->ExpressMail( FRAME_SCH, MAIL_RELOAD_LIB, payload, nullptr, true );
        aKiway->ExpressMail( FRAME_SCH_SYMBOL_EDITOR, MAIL_RELOAD_LIB, payload, nullptr, true );
        aKiway->ExpressMail( FRAME_SCH_VIEWER, MAIL_RELOAD_LIB, payload, nullptr, true );
        return adapter->LoadSymbol( id );
    };


    const auto full_symbol_file_path = *GetProjectResourceSavePath( SYMBOL_DIR, symbol_file_name );
    // NOTE : Do not write symbol lib table, it will crash the symbol chooser
    auto lib_id = WriteSymbolFields( symbol_file_name, full_symbol_file_path, symbol_nick_name, aComponent.attributes,
                                     RemoveFileExt( aComponent.footprints.size()
                                                            ? aComponent.footprints.front().footprint.filename
                                                            : aComponent.symbol.filename ) );

    if( !lib_id )
    {
        return {};
    }

    auto lib_sym = do_AddLibrary( full_symbol_file_path, *lib_id );


    if( !UpdateLibTable( LIBRARY_TABLE_TYPE::FOOTPRINT, GetAPIFootprintPrettyName(), GetProjectApiFpUri() ) )
    {
        wxLogWarning( "Failed to update fp library table for %s", symbol_nick_name );
    }

    if( !ReloadFootprintLib( GetAPIFootprintPrettyName() ) )
    {
        wxLogWarning( "Failed to reload fp library for %s", symbol_nick_name );
    }

    return lib_sym;
}

bool API_SCH_DESIGN_RESOURCE_MGR::SaveDesignBlockToProject( DESIGN_BLOCK_CONTENT const& aBlock )
{
    for( const auto& component : aBlock.components )
    {
        SaveComponentToProject( component );
    }

    for( const auto [schematic_file_name, schematic_content] : aBlock.schematics )
    {
        if( !WriteProjectResource( PROJECT_ROOT, schematic_file_name, schematic_content ) )
        {
            wxLogError( "Failed to save schematic %s to project", schematic_file_name );
            return {};
        }
    }

    if( aBlock.pcb )
    {
        if( !WriteProjectResource( PROJECT_ROOT, aBlock.pcb->pcb.filename, aBlock.pcb->pcb.content ) )
        {
            wxLogError( "Failed to save pcb %s to project", aBlock.pcb->pcb.filename );
            return {};
        }
    }

    return true;
}

std::optional<LIB_ID> API_SCH_DESIGN_RESOURCE_MGR::WriteSymbolFields( wxString const& aFileName,
                                                                      wxString const& aFilePath,
                                                                      wxString const& aNickName, auto const& aFields,
                                                                      auto const& aFootprintName )
{
    LIB_SYMBOL* lib_sym{};

    auto full_fp = ( wxFileName( *GetProjectResourceSavePath( SYMBOL_DIR, aFileName ) ) ).GetFullPath();


    try
    {
        std::vector<LIB_SYMBOL*> symbols;
        m_schIo->EnumerateSymbolLib( symbols, full_fp );

        if( symbols.empty() )
        {
            wxLogError( "Symbol '%s' not found in library table.", aNickName );
            return {};
        }

        lib_sym = symbols.front();
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Error loading symbol library '%s'." ) + wxS( "\n%s" ), aNickName, ioe.What() );
        return {};
    }

    LIB_ID id;
    id.SetLibNickname( aNickName );
    id.SetLibItemName( lib_sym->GetName() );
    lib_sym->SetLibId( id );

    std::map<std::string, std::string> attrs;

    for( auto& [name, text] : aFields )
    {
        attrs.try_emplace( name, text );
    }

    std::vector<SCH_FIELD> fields;

    // FIXME : Deleting the new SCH_FIELD will crash the program
    if( attrs.find( kMPN ) != attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, FIELD_T::USER, kMPN );
        field->SetText( attrs[kMPN] );
        fields.push_back( *field );
    }

    if( attrs.find( kMANUFACTURER ) != attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, FIELD_T::USER, kMANUFACTURER );
        field->SetText( attrs[kMANUFACTURER] );
        fields.push_back( *field );
    }

    for( auto& [name, text] : attrs )
    {
        SCH_FIELD* field = nullptr;

        if( name == kVALUE )
        {
            field = new SCH_FIELD( lib_sym->GetValueField() );
        }
        else if( name == kFOOTPRINT )
        {
            continue;
        }
        else if( name == kDATASHEET )
        {
            field = new SCH_FIELD( lib_sym->GetDatasheetField() );
        }
        else if( name == kDESCRIPTION )
        {
            field = new SCH_FIELD( lib_sym->GetDescriptionField() );
        }
        else if( name != kMANUFACTURER && name != kMPN )
        {
            field = new SCH_FIELD( lib_sym, FIELD_T::USER, name );
        }
        if( name != kMANUFACTURER && name != kMPN )
        {
            field->SetText( text );
            fields.push_back( *field );
        }
    }

    SCH_FIELD* ref_field = new SCH_FIELD( lib_sym->GetReferenceField() );
    fields.push_back( *ref_field );
    lib_sym->GetFootprintField().SetText( FormatSymbolFpPropertyValue( aFootprintName ) );
    fields.push_back( lib_sym->GetFootprintField() );

    for( auto& field : fields )
    {
        const auto is_value_ref = field.GetId() == FIELD_T::REFERENCE || field.GetId() == FIELD_T::VALUE;
        field.SetVisible( is_value_ref );
        field.SetShowInChooser( is_value_ref );
    }


    lib_sym->SetHqPartsFields( fields );


    try
    {
        // FIXME: Need to call Duplicate here
        m_schIo->SaveSymbol( aFilePath, lib_sym->Duplicate() );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Failed to save library %s. %s" ), aFilePath, ioe.What() );
        return {};
    }

    return id;
}
