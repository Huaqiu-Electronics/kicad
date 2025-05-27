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
#include <project_sch.h>
#include <api/api_handler_sch.h>
#include <api/api_sch_utils.h>
#include <api/api_utils.h>
#include <magic_enum.hpp>
#include <sch_commit.h>
#include <sch_edit_frame.h>
#include <wx/filename.h>

#include <api/common/types/base_types.pb.h>
#include <api/api_design_resource.h>
#include <wx/log.h>
#include <fmt/format.h>


using namespace kiapi::common::commands;
using kiapi::common::types::CommandStatus;
using kiapi::common::types::DocumentType;
using kiapi::common::types::ItemRequestStatus;

HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceDesignBlock( const HANDLER_CONTEXT<PlaceDesignBlock>& aCtx )
{
    if( !aCtx.Request.has_design_block_resource() || aCtx.Request.design_block_resource().schematic_uris().empty() )
        return {};


    DesignBlockConent design_block_content;

    if( aCtx.Request.design_block_resource().has_root_schematic() )
    {
        design_block_content.root = aCtx.Request.design_block_resource().root_schematic();
    }

    for( const auto& uri : aCtx.Request.design_block_resource().schematic_uris() )
    {
        NamedContent schematic;
        const auto   schematic_content = API_DESIGN_RESOURCE_MGR::DownloadURL( uri );

        if( !schematic_content )
        {
            wxLogWarning( "Failed to download schematic from %s", uri );
            continue;
        }

        schematic.filename = API_DESIGN_RESOURCE_MGR::ExtractFileName( uri );
        schematic.content = *schematic_content;
        design_block_content.schematics.push_back( std::move( schematic ) );
    }

    // TODO : handle the PCB
    handlePlaceDesignBlock( design_block_content );
    return {};
}


void API_HANDLER_SCH::handlePlaceDesignBlock( struct DesignBlockConent const& aBlock )
{
    m_resourceMgr->SaveDesignBlockToProject( aBlock );
}


HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceSymbol( const HANDLER_CONTEXT<PlaceSymbol>& aCtx )
{
    if( !aCtx.Request.has_component_resource() || !aCtx.Request.component_resource().has_symbol_resource()
        || aCtx.Request.component_resource().symbol_resource().uri().empty() )
        return {};


    const auto symbol_uri = aCtx.Request.component_resource().symbol_resource().uri();
    const auto symbol_content = API_DESIGN_RESOURCE_MGR::DownloadURL( symbol_uri );

    if( !symbol_content )
    {
        wxLogWarning( "Failed to download symbol from %s", symbol_uri );
        return {};
    }


    const auto symbol_name = aCtx.Request.component_resource().has_component_name()
                                     ? aCtx.Request.component_resource().component_name()
                                     : API_DESIGN_RESOURCE_MGR::ExtractFileName( symbol_uri );

    ComponentContent component_content{
        fmt::format( "{}.{}", symbol_name, API_DESIGN_RESOURCE_MGR::ExtactFileExtension( symbol_uri ) ), *symbol_content
    };

    for( const auto& [k, v] : aCtx.Request.component_resource().attributes() )
        component_content.attributes[k] = v;

    for( const auto& fp : aCtx.Request.component_resource().footprint_resources() )
    {
        FootprintConent footprint_resource;
        const auto      fp_uri = fp.uri();
        const auto      fp_content = API_DESIGN_RESOURCE_MGR::DownloadURL( fp_uri );

        if( !fp_content )
        {
            wxLogWarning( "Failed to download footprint from %s", fp_uri );
            continue;
        }

        footprint_resource.footprint.filename =
                component_content.footprints.empty()
                        ? fmt::format( "{}.{}", symbol_name, API_DESIGN_RESOURCE_MGR::ExtactFileExtension( fp_uri ) )
                        : API_DESIGN_RESOURCE_MGR::ExtractFileName( fp_uri );

        footprint_resource.footprint.content = *fp_content;

        for( const auto& ml : fp.models() )
        {
            NamedContent model;
            const auto   model_uri = ml.uri();
            const auto   model_content = API_DESIGN_RESOURCE_MGR::DownloadURL( model_uri );

            if( !model_content )
            {
                wxLogWarning( "Failed to download 3D model from %s", model_uri );
                continue;
            }

            model.filename = component_content.footprints.empty()
                                 ? fmt::format( "{}.{}", symbol_name,
                                                API_DESIGN_RESOURCE_MGR::ExtactFileExtension( model_uri ) )
                                 : API_DESIGN_RESOURCE_MGR::ExtractFileName( model_uri );
            model.content = *model_content;
            footprint_resource.models.push_back( model );
        }

        component_content.footprints.push_back( std::move( footprint_resource ) );
    }

    handlePlaceSymbol( component_content );
    return {};
}


void API_HANDLER_SCH::handlePlaceSymbol( struct ComponentContent const& aComponent )
{
    // Trigger loading the lazy ininitialized project symbol library table.
    void( PROJECT_SCH::SchSymbolLibTable( &m_frame->Prj() ) );


    if( !m_resourceMgr->SaveComponentToProject( aComponent ) )
    {
        wxLogError( "Failed to save component to project" );
        return;
    }
}
