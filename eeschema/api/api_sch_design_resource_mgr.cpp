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
#include "sch_base_frame.h"
#include <functional>
#include <lib_symbol.h>
#include <sch_field.h>
#include <api/api_design_resource.h>
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
    FIELD_WRITER( LIB_SYMBOL* aLibSym, std::vector<SCH_FIELD>& aFields ) :
            m_lib( aLibSym ), m_fields( aFields )
    {
    }

    auto add_field( SCH_FIELD&& field, const wxString& aText )
    {
        field.SetShowInChooser( false );
        field.SetVisible( false );
        field.SetText( aText );
        m_fields.push_back( std::move( field ) );
    }

    auto add_field( const SCH_FIELD& other, const wxString& aText )
    {
        add_field( SCH_FIELD( other ), aText );
    }

    auto add_field( const wxString& aName, const wxString& aText )
    {
        add_field( SCH_FIELD( m_lib, {}, aName ), aText );
    };
};


API_SCH_DESIGN_RESOURCE_MGR::API_SCH_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) :
        API_DESIGN_RESOURCE_MGR( aFrame )
{
}

API_SCH_DESIGN_RESOURCE_MGR::~API_SCH_DESIGN_RESOURCE_MGR()
{
}


LIB_SYMBOL*
API_SCH_DESIGN_RESOURCE_MGR::SaveComponentToProject( COMPONENT_CONTENT const& aComponent )
{
    const auto& symbol_file_name = aComponent.symbol.filename;

    if( !WriteProjectResource( SYMBOL_DIR, symbol_file_name, aComponent.symbol.content ) )
    {
        wxLogError( "Failed to save symbol %s to project", symbol_file_name );
        return {};
    }

    for( const auto& footprint : aComponent.footprints )
    {
        if( !WriteProjectResource( PRETTY_DIR, footprint.footprint.filename,
                                   footprint.footprint.content ) )
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

    if( !UpdateLibTable( API_LIB_SYMBOL_TABLE, symbol_nick_name,
                         FormatAPISymbolURI( symbol_file_name ) ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }


    LIB_SYMBOL* lib_sym = WriteSymbolFields( symbol_nick_name, aComponent.attributes );

    if( !lib_sym )
    {
        wxLogError( "Failed to write symbol fields for %s", symbol_nick_name );
        return {};
    }


    if( !UpdateLibTable( API_LIB_FP_TABLE, GetAPIFootprintPrettyName(), GetProjectApiFpUri() ) )
    {
        wxLogError( "Failed to update fp library table for %s", symbol_nick_name );
        return {};
    }

    return lib_sym;
}

bool API_SCH_DESIGN_RESOURCE_MGR::SaveDesignBlockToProject( DESIGN_BLOCK_CONTENT const& aComponent )
{
    return {};
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

LIB_SYMBOL* API_SCH_DESIGN_RESOURCE_MGR::WriteSymbolFields( std::string const& aNickName,
                                                            auto const&        aFields )
{
    const auto symbolTb = PROJECT_SCH::SchSymbolLibTable( &m_frame.Prj() );

    if( !symbolTb )
    {
        wxLogError( "Failed to get symbol library table." );
        return {};
    }

    LIB_SYMBOL* lib_sym = nullptr;

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
        wxLogError( _( "Error loading HQ symbol library '%s'." ) + wxS( "\n%s" ), aNickName,
                    ioe.What() );
        return {};
    }

    lib_sym->SetName( aNickName );
    std::vector<SCH_FIELD> fields;
    lib_sym->CopyFields( fields );

    FIELD_WRITER field_writer( lib_sym, fields );

    std::map<std::string, std::reference_wrapper<SCH_FIELD>> field_map;

    for( auto& field : fields )
        field_map.emplace( field.GetName(), std::ref( field ) );


    for( const auto& field : aFields )
    {
        if( auto loc = field_map.find( field.name ); loc != field_map.end() )
        {
            loc->second.get().SetText( field.value );
        }
        else
        {
            field_writer.add_field( field.name, field.value );
        }
    }

    lib_sym->SetFields( fields );
    lib_sym->GetFootprintField().SetText(FormatSymbolFpPropertyValue(aNickName));

    if( auto res = symbolTb->SaveSymbol( aNickName, lib_sym, true );
        res != SYMBOL_LIB_TABLE::SAVE_OK )
    {
        wxLogError( "Failed to save symbol fields for %s", aNickName );
        return {};
    }

    return lib_sym;
}
