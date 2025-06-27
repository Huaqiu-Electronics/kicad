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

#include "api_sch_design_resource_mgr.h"
#include "api/api_design_resource_mgr.h"
#include "sch_base_frame.h"
#include "sch_field_names.h"
#include <lib_symbol.h>
#include <sch_field.h>
#include <api/api_design_content.h>
#include <string>
#include <symbol_lib_table.h>
#include <eda_base_frame.h>
#include <project_sch.h>
#include <wx/log.h>

class FIELD_WRITER
{
    LIB_SYMBOL*             m_lib;
    std::vector<SCH_FIELD>& m_fields;

public:
    FIELD_WRITER( LIB_SYMBOL* aLibSym, std::vector<SCH_FIELD>& aFields ) : m_lib( aLibSym ), m_fields( aFields ) {}

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


API_SCH_DESIGN_RESOURCE_MGR::API_SCH_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) : API_DESIGN_RESOURCE_MGR( aFrame )
{
}

API_SCH_DESIGN_RESOURCE_MGR::~API_SCH_DESIGN_RESOURCE_MGR()
{
}


LIB_SYMBOL* API_SCH_DESIGN_RESOURCE_MGR::SaveComponentToProject( COMPONENT_CONTENT const& aComponent )
{
    const auto& symbol_file_name = aComponent.symbol.filename;

    if( !WriteProjectResource( SYMBOL_DIR, symbol_file_name, aComponent.symbol.content ) )
    {
        wxLogError( "Failed to save symbol %s to project", symbol_file_name );
        return {};
    }

    for( const auto& footprint : aComponent.footprints )
    {
        if( !WriteProjectResource( PRETTY_DIR, footprint.footprint.filename, footprint.footprint.content ) )
        {
            wxLogError( "Failed to save footprint %s to project", footprint.footprint.filename );
            continue;
        }

        for( const auto& model : footprint.models )
        {
            if( !WriteProjectResource( MODEL_3D_DIR, model.filename, model.content ) )
            {
                wxLogError( "Failed to save 3D model %s to project", model.filename );
                continue;
            }
        }
    }


    const auto symbol_nick_name = RemoveFileExt( symbol_file_name );

    if( !UpdateLibTable( API_LIB_SYMBOL_TABLE, symbol_nick_name, FormatAPISymbolURI( symbol_file_name ) ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }

    auto lib_id = WriteSymbolFields( symbol_file_name, aComponent.attributes );

    if( !UpdateLibTable( API_LIB_SYMBOL_TABLE, symbol_nick_name, FormatAPISymbolURI( symbol_file_name ) ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }

    auto lib_sym = SchGetLibSymbol( lib_id, PROJECT_SCH::SchSymbolLibTable( &m_frame.Prj() ), nullptr, &m_frame );

    if( !lib_sym )
    {
        wxLogError( "Failed to load symbol for %s", symbol_nick_name );
        return {};
    }


    if( !UpdateLibTable( API_LIB_FP_TABLE, GetAPIFootprintPrettyName(), GetProjectApiFpUri() ) )
    {
        wxLogError( "Failed to update fp library table for %s", symbol_nick_name );
        return {};
    }

    return lib_sym;
}

bool API_SCH_DESIGN_RESOURCE_MGR::SaveDesignBlockToProject( DESIGN_BLOCK_CONTENT const& aBlock )
{
    for( const auto& component : aBlock.components )
        SaveComponentToProject( component );

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


bool API_SCH_DESIGN_RESOURCE_MGR::UpdateLibTable( API_LIB_TABLE_T aLibT, const wxString& aNickName,
                                                  const wxString& aURI )
{
    static const auto libType = wxT( "KiCad" );
    static const auto desc = wxT( "Added by schematic API" );

    switch( aLibT )
    {
    case API_SCH_DESIGN_RESOURCE_MGR::API_LIB_SYMBOL_TABLE:
    {
        const auto symbolTb = PROJECT_SCH::SchSymbolLibTable( &m_frame.Prj() );

        if( !symbolTb )
        {
            wxLogError( "Failed to get symbol library table." );
            return false;
        }

        const auto row = new SYMBOL_LIB_TABLE_ROW( aNickName, aURI, libType, desc );

        if( !symbolTb->InsertRow( row, true ) )
        {
            wxLogError( "Failed to add symbol library row: %s", aNickName );
            return false;
        }

        symbolTb->Save( m_frame.Prj().SymbolLibTableName() );
        static_cast<SCH_BASE_FRAME&>( m_frame ).SavePrjSymbolLibTables();
        return true;
    }
    case API_SCH_DESIGN_RESOURCE_MGR::API_LIB_FP_TABLE:
    {
        // Update the footprint library table using the global function.
        return m_frame.Prj().InsertFpLibTableRow( m_frame.Kiway(), aNickName, aURI, libType,
                                                  m_frame.Prj().FootprintLibTblName(), desc );
    }
    case API_SCH_DESIGN_RESOURCE_MGR::API_LIB_DESIGN_BLOCK_TABLE:
    {
        // TODO
        break;
    }
    }


    return false;
}

LIB_ID API_SCH_DESIGN_RESOURCE_MGR::WriteSymbolFields( std::string const& aFileName, auto const& aFields )
{
    const auto symbolTb = PROJECT_SCH::SchSymbolLibTable( &m_frame.Prj() );

    if( !symbolTb )
    {
        wxLogError( "Failed to get symbol library table." );
        return {};
    }

    LIB_SYMBOL* lib_sym = nullptr;

    const auto aNickName = RemoveFileExt( aFileName );

    try
    {
        std::vector<LIB_SYMBOL*> symbols;
        symbolTb->LoadSymbolLib( symbols, aNickName, false );

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
    int                    field_id = 5;
    // Set fields order by manual.
    if( attrs.find( kMPN ) != attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, field_id++, kMPN );
        // field->SetId(  );
        // field->SetName();
        field->SetShowInChooser( false );
        field->SetVisible( false );
        field->SetText( attrs[kMPN] );
        fields.push_back( *field );
    }

    if( attrs.find( kMANUFACTURER ) != attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, field_id++, kMANUFACTURER );
        // field->SetId( field_id++ );
        // field->SetName( MANUFACTURER );
        field->SetShowInChooser( false );
        field->SetVisible( false );
        field->SetText( attrs[kMANUFACTURER] );
        fields.push_back( *field );
    }

    for( auto& [name, text] : attrs )
    {
        SCH_FIELD* field = nullptr;
        if( name == kVALUE )
        {
            field = new SCH_FIELD( lib_sym->GetValueField() );
            field->SetVisible( false );
        }
        else if( name == kFOOTPRINT )
        {
            continue;
        }
        else if( name == kDATASHEET )
        {
            field = new SCH_FIELD( lib_sym->GetDatasheetField() );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }
        else if( name == kDESCRIPTION )
        {
            field = new SCH_FIELD( lib_sym->GetDescriptionField() );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }
        else if( name != kMANUFACTURER && name != kMPN )
        {
            field = new SCH_FIELD( lib_sym, field_id++, name );
            // field->SetId( field_id++ );
            // field->SetName( name );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }

        if( name != kMANUFACTURER && name != kMPN )
        {
            field->SetText( text );
            fields.push_back( *field );
        }
    }
    SCH_FIELD* ref_field = new SCH_FIELD( lib_sym->GetReferenceField() );
    ref_field->SetShowInChooser( true );
    ref_field->SetVisible( true );
    fields.push_back( *ref_field );
    lib_sym->GetFootprintField().SetText( FormatSymbolFpPropertyValue( aNickName ) );
    fields.push_back( lib_sym->GetFootprintField() );
    // replace old fields
    lib_sym->SetHqPartsFields( fields );


    const auto fp = *GetProjectResourceSavePath( SYMBOL_DIR, aFileName );
    wxFileName fn( fp );

    try
    {
        IO_RELEASER<SCH_IO> pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_KICAD ) );
        pi->DeleteSymbol( fn.GetFullPath(), old_symName );
        pi->SaveSymbol( fn.GetFullPath(), new LIB_SYMBOL( *lib_sym ) );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Failed to save library %s. %s" ), fn.GetFullPath(), ioe.What() );
        return {};
    }

    return lib_sym->GetLibId();
}
