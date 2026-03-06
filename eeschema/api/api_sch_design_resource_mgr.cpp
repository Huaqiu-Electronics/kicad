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
#include <wx/log.h>

#include "api_sch_design_resource_mgr.h"
#include "api/api_design_resource_mgr.h"
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
        m_schIo( std::unique_ptr<SCH_IO>( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_KICAD ) ) )
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
        return {};


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
    const auto symbol_uri = FormatAPISymbolURI( symbol_file_name );

    if( !UpdateLibTable( LIBRARY_TABLE_TYPE::SYMBOL, symbol_nick_name, symbol_uri ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }


    auto lib_id = WriteSymbolFields( symbol_file_name, symbol_nick_name, aComponent.attributes,
                                     RemoveFileExt( aComponent.footprints.size()
                                                            ? aComponent.footprints.front().footprint.filename
                                                            : aComponent.symbol.filename ) );

    if( !lib_id )
    {
        wxLogError( "Failed to write symbol fields for %s", symbol_nick_name );
        return {};
    }


    auto lib_sym = SchGetLibSymbol( *lib_id, PROJECT_SCH::SymbolLibAdapter( &m_frame.Prj() ), nullptr, &m_frame );

    if( !lib_sym )
    {
        wxLogError( "Failed to load symbol for %s", symbol_nick_name );
        return {};
    }


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

std::optional<LIB_ID> API_SCH_DESIGN_RESOURCE_MGR::WriteSymbolFields( std::string const& aFileName,
                                                                      std::string const& aNickName, auto const& aFields,
                                                                      auto const& aFootprintName )
{
    LIB_SYMBOL* lib_sym{};

    auto adapter = PROJECT_SCH::SymbolLibAdapter( &m_frame.Prj() );

    try
    {
        auto res = adapter->LoadOne( aNickName );

        if( !res || res->load_status == LOAD_STATUS::INVALID )
        {
            wxLogError( _( "Failed to load symbol library '%s'." ), aNickName );
            return {};
        }

        auto symbols = adapter->GetSymbols( aNickName, SYMBOL_LIBRARY_ADAPTER::SYMBOL_TYPE::ALL_SYMBOLS );
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

    wxString old_symName = lib_sym->GetName();
    lib_sym->SetName( aNickName );

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

    const auto fp = *GetProjectResourceSavePath( SYMBOL_DIR, aFileName );
    wxFileName fn( fp );

    try
    {
        m_schIo->DeleteSymbol( fn.GetFullPath(), old_symName );
        m_schIo->SaveSymbol( fn.GetFullPath(), new LIB_SYMBOL( *lib_sym ) );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Failed to save library %s. %s" ), fn.GetFullPath(), ioe.What() );
        return {};
    }

    // NOTE : The lib sym will be invalidate after reloading
    const auto lib_id_cp = lib_sym->GetLibId();

    Pgm().GetLibraryManager().ReloadLibraryEntry( LIBRARY_TABLE_TYPE::SYMBOL, aNickName, LIBRARY_TABLE_SCOPE::PROJECT );

    if( auto status = adapter->LoadOne( aNickName ); !status || status->load_status != LOAD_STATUS::LOADED )
    {
        wxLogError( _( "Failed to reload symbol library '%s' after saving." ), aNickName );
        return {};
    }

    return lib_id_cp;
}
