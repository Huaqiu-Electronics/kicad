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

#include "api_pcb_design_resource_mgr.h"
#include "footprint.h"
#include <optional>
#include <project.h>
#include <api/api_fmt_fp_model_path.h>
#include <api/api_design_resource.h>
#include <api/api_design_content.h>
#include <eda_base_frame.h>
#include <project_pcb.h>
#include <wx/log.h>
#include <fp_lib_table.h>

API_PCB_DESIGN_RESOURCE_MGR::API_PCB_DESIGN_RESOURCE_MGR( EDA_BASE_FRAME& aFrame ) : API_DESIGN_RESOURCE_MGR( aFrame )
{
}

API_PCB_DESIGN_RESOURCE_MGR::~API_PCB_DESIGN_RESOURCE_MGR()
{
}

FOOTPRINT* API_PCB_DESIGN_RESOURCE_MGR::SaveComponentToProject( COMPONENT_CONTENT const& aComponent )
{
    const auto& symbol_file_name = aComponent.symbol.filename;

    if( !WriteProjectResource( SYMBOL_DIR, symbol_file_name, aComponent.symbol.content ) )
    {
        wxLogError( "Failed to save symbol %s to project", symbol_file_name );
    }

    auto libTable = PROJECT_PCB::PcbFootprintLibs( &m_frame.Prj() );

    if( !libTable )
    {
        wxLogError( "Failed to get footprint library table" );
        return {};
    }


    std::optional<FOOTPRINT*> footprint;

    for( const auto& footprint : aComponent.footprints )
    {
        std::optional<std::string> model_name;

        for( const auto& model : footprint.models )
        {
            if( !model_name )
            {
                model_name = model.filename;
            }

            if( !WriteProjectResource( MODEL_3D_DIR, model.filename, model.content ) )
            {
                wxLogError( "Failed to save 3D model %s to project", model.filename );
                continue;
            }
        }

        if( !WriteProjectResource( PRETTY_DIR, footprint.footprint.filename, footprint.footprint.content ) )
        {
            wxLogError( "Failed to save footprint %s to project", footprint.footprint.filename );
            continue;
        }


        if( model_name )
        {
            m_frame.Prj().UpdateFootprintModelPath(
                    m_frame.Kiway(), *GetProjectResourceSavePath( PRETTY_DIR, footprint.footprint.filename ),
                    FormatFootprintModelPath( model_name.value() ) );
        }
    }


    const auto symbol_nick_name = RemoveFileExt( symbol_file_name );

    if( !UpdateLibTable( API_LIB_SYMBOL_TABLE, symbol_nick_name, FormatAPISymbolURI( symbol_file_name ) ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }

    auto lib_id = WriteFootprintFields( symbol_file_name, aComponent.attributes,
                                        RemoveFileExt( aComponent.footprints.size()
                                                               ? aComponent.footprints.front().footprint.filename
                                                               : aComponent.symbol.filename ) );

    if( !UpdateLibTable( API_LIB_SYMBOL_TABLE, symbol_nick_name, FormatAPISymbolURI( symbol_file_name ) ) )
    {
        wxLogError( "Failed to update symbol library table for %s", symbol_nick_name );
        return {};
    }


    auto fp = libTable->FootprintLoad( GetAPIFootprintPrettyName(), GetProjectApiFpUri(), true );


    if( !fp )
    {
        wxLogError( "Failed to load fp for %s", symbol_nick_name );
        return {};
    }


    if( !UpdateLibTable( API_LIB_FP_TABLE, GetAPIFootprintPrettyName(), GetProjectApiFpUri() ) )
    {
        wxLogError( "Failed to update fp library table for %s", symbol_nick_name );
        return {};
    }

    return fp;
}

bool API_PCB_DESIGN_RESOURCE_MGR::UpdateLibTable( API_LIB_TABLE_T aLibT, const wxString& aNickName,
                                                  const wxString& aURI )
{
    static const auto libType = wxT( "KiCad" );
    static const auto desc = wxT( "Added by PCB API" );

    switch( aLibT )
    {
    case API_LIB_TABLE_T::API_LIB_FP_TABLE:
    {
        // Update the footprint library table using the global function.
        return m_frame.Prj().InsertFpLibTableRow( m_frame.Kiway(), aNickName, aURI, libType,
                                                  m_frame.Prj().FootprintLibTblName(), desc );
    }
    default:
    {
        wxLogWarning( "Unsupported library table type" );
    }
    }

    return {};
}

LIB_ID API_PCB_DESIGN_RESOURCE_MGR::WriteFootprintFields( std::string const& aFileName, auto const& aFields,
                                                          auto const& aFootprintName )
{
    return {};
}
