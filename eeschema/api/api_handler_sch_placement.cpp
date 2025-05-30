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
#include <api/api_design_resource.h>
#include "api_sch_placement_command.h"
#include <api/api_design_content.h>
#include <sch_actions.h>
#include <tool/tool_manager.h>
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


namespace
{

COMPONENT_RESOURCE    ConvertProtoToComponentResource( const kiapi::common::types::ComponentResource& protoComponent );
FOOTPRINT_RESOURCE    ConvertProtoToFootprintResource( const kiapi::common::types::FootprintResource& protoFp );
PCB_RESOURCE          ConvertProtoToPcbResource( const kiapi::common::types::PCBResource& protoPcb );
DESIGN_BLOCK_RESOURCE ConvertProtoToDesignBlockResource( const kiapi::common::types::DesignBlockResource& protoBlock );

COMPONENT_RESOURCE ConvertProtoToComponentResource( const kiapi::common::types::ComponentResource& protoComponent )
{
    COMPONENT_RESOURCE component;

    if( protoComponent.has_component_name() )
        component.component_name = protoComponent.component_name();

    if( protoComponent.has_symbol_resource() )
        component.symbol_resource.uri = protoComponent.symbol_resource().uri();

    for( const auto& proto_fp : protoComponent.footprint_resources() )
        component.footprint_resources.push_back( ConvertProtoToFootprintResource( proto_fp ) );

    for( const auto& proto_attr : protoComponent.attributes() )
    {
        component.attributes.push_back( { proto_attr.name(), proto_attr.value() } );
    }

    return component;
}


FOOTPRINT_RESOURCE ConvertProtoToFootprintResource( const kiapi::common::types::FootprintResource& protoFp )
{
    FOOTPRINT_RESOURCE fp;

    if( protoFp.has_uri() )
        fp.uri = protoFp.uri();

    for( const auto& proto_model : protoFp.models() )
        fp.models.push_back( { proto_model.uri() } );

    return fp;
}


PCB_RESOURCE ConvertProtoToPcbResource( const kiapi::common::types::PCBResource& protoPcb )
{
    PCB_RESOURCE pcb;

    pcb.pcb_uri = protoPcb.pcb_uri();

    for( const auto& proto_fp : protoPcb.footprint_resources() )
        pcb.footprint_resources.push_back( ConvertProtoToFootprintResource( proto_fp ) );

    return pcb;
}


DESIGN_BLOCK_RESOURCE ConvertProtoToDesignBlockResource( const kiapi::common::types::DesignBlockResource& protoBlock )
{
    DESIGN_BLOCK_RESOURCE resource;

    for( const auto& uri : protoBlock.schematic_uris() )
        resource.schematic_uris.push_back( uri );

    if( protoBlock.has_root_schematic() )
        resource.root_schematic = protoBlock.root_schematic();

    for( const auto& proto_component : protoBlock.component_resources() )
        resource.component_resources.push_back( ConvertProtoToComponentResource( proto_component ) );

    if( protoBlock.has_pcb_resource() )
        resource.pcb_resource = ConvertProtoToPcbResource( protoBlock.pcb_resource() );

    if( protoBlock.has_version() )
        resource.version = protoBlock.version();

    return resource;
}


} // namespace


void API_HANDLER_SCH::placeDesignBlock( PLACE_DESIGN_BLOCK_RESOURCE_COMMAND const& aCommand )
{
    DESIGN_BLOCK_CONTENT design_block_content;

    const auto& design_block = aCommand.design_block;
    if( design_block.root_schematic )
    {
        design_block_content.root = *design_block.root_schematic;
    }

    for( const auto& uri : design_block.schematic_uris )
    {
        NAMED_CONTENT schematic;
        const auto    schematic_content = API_DESIGN_RESOURCE_MGR::DownloadURL( uri );

        if( !schematic_content )
        {
            wxLogWarning( "Failed to download schematic from %s", uri );
            continue;
        }

        schematic.filename = API_DESIGN_RESOURCE_MGR::ExtractFileName( uri );
        schematic.content = *schematic_content;
        design_block_content.schematics.push_back( std::move( schematic ) );
    }

    placeDesignBlock( design_block_content, aCommand.option );
}


HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceDesignBlock( const HANDLER_CONTEXT<PlaceDesignBlock>& aCtx )
{
    if( !aCtx.Request.has_design_block_resource() || aCtx.Request.design_block_resource().schematic_uris().empty() )
    {
        return {};
    }

    placeDesignBlock(
            { ConvertProtoToDesignBlockResource( aCtx.Request.design_block_resource() ),
              { aCtx.Request.has_place_as_sheet() ? aCtx.Request.place_as_sheet() : std::optional<bool>{} } } );
    return {};
}


void API_HANDLER_SCH::placeDesignBlock( DESIGN_BLOCK_CONTENT const& aBlock, PLACE_DESIGN_BLOCK_OPTION const& aOption )
{
    if( !aBlock.schematics.size() )
        return;

    if( !m_resourceMgr->SaveDesignBlockToProject( aBlock ) )
        return;

    wxString target_sch_file =
            m_resourceMgr
                    ->GetProjectResourceSavePath( API_DESIGN_RESOURCE_MGR::ProjectResourceDir::PROJECT_ROOT,
                                                  aBlock.root ? *aBlock.root : aBlock.schematics.front().filename )
                    .value();

    wxFileName rootFile( target_sch_file );

    if( !rootFile.FileExists() )
    {
        wxLogError( wxString::Format( "File %s does not exist", rootFile.GetFullPath() ) );
        return;
    }


    m_frame->GetToolManager()->RunAction<SCH_ACTIONS::PLACE_SHEET_PARAMS>(
            SCH_ACTIONS::apiPlaceSheet,
            { target_sch_file, aOption.place_as_sheet.has_value() ? aOption.place_as_sheet.value() : false } );
}


void API_HANDLER_SCH::placeSymbol( PLACE_SYMBOL_RESOURCE_COMMAND const& aCommand )
{
    const auto& component = aCommand.component;

    if( component.symbol_resource.uri.empty() )
        return;


    const auto& symbol_uri = component.symbol_resource.uri;
    const auto  symbol_content = API_DESIGN_RESOURCE_MGR::DownloadURL( symbol_uri );

    if( !symbol_content )
    {
        wxLogWarning( "Failed to download symbol from %s", symbol_uri );
        return;
    }


    const auto symbol_name = API_DESIGN_RESOURCE_MGR::FormatToValidFileName(
            component.component_name ? *component.component_name
                                     : API_DESIGN_RESOURCE_MGR::ExtractFileName( symbol_uri ) );

    COMPONENT_CONTENT component_content{
        fmt::format( "{}.{}", symbol_name, API_DESIGN_RESOURCE_MGR::ExtactFileExtension( symbol_uri ) ), *symbol_content
    };
    for( const auto& attr : component.attributes )
        component_content.attributes.push_back( { attr.name, attr.value } );

    for( const auto& fp : component.footprint_resources )
    {
        if( !fp.uri || fp.uri->empty() )
            continue;

        FOOTPRINT_CONENT footprint_resource;
        const auto       fp_uri = *fp.uri;
        const auto       fp_content = API_DESIGN_RESOURCE_MGR::DownloadURL( fp_uri );

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

        for( const auto& ml : fp.models )
        {
            NAMED_CONTENT model;

            const auto model_uri = ml.uri;
            const auto model_content = API_DESIGN_RESOURCE_MGR::DownloadURL( model_uri );

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

    placeSymbol( component_content, aCommand.option );
}

HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceSymbol( const HANDLER_CONTEXT<PlaceSymbol>& aCtx )
{
    if( !aCtx.Request.has_component_resource() || !aCtx.Request.component_resource().has_symbol_resource()
        || aCtx.Request.component_resource().symbol_resource().uri().empty() )
    {
        return {};
    }

    placeSymbol( { ConvertProtoToComponentResource( aCtx.Request.component_resource() ), {} } );
    return {};
}


void API_HANDLER_SCH::placeSymbol( COMPONENT_CONTENT const& aComponent, PLACE_SYMBOL_OPTION const& aOption )
{
    // Trigger loading the lazy ininitialized project symbol library table.
    void( PROJECT_SCH::SchSymbolLibTable( &m_frame->Prj() ) );
    auto libSymbol = m_resourceMgr->SaveComponentToProject( aComponent );

    if( !libSymbol )
    {
        wxLogError( "Failed to save component to project" );
        return;
    }

    PICKED_SYMBOL pickedSymbol;
    pickedSymbol.LibId = libSymbol->GetLIB_ID();
    m_frame->GetToolManager()->RunAction<SCH_ACTIONS::PLACE_SYMBOL_PARAMS>(
            SCH_ACTIONS::placeSymbol,
            { new SCH_SYMBOL( *libSymbol, &m_frame->GetCurrentSheet(), pickedSymbol, {}, &m_frame->Schematic() ), true,
              true } );
}
