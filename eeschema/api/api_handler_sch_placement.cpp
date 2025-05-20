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

#include <api/api_handler_sch.h>
#include <api/api_sch_utils.h>
#include <api/api_utils.h>
#include <magic_enum.hpp>
#include <sch_commit.h>
#include <sch_edit_frame.h>
#include <wx/filename.h>

#include <api/common/types/base_types.pb.h>

using namespace kiapi::common::commands;
using kiapi::common::types::CommandStatus;
using kiapi::common::types::DocumentType;
using kiapi::common::types::ItemRequestStatus;


namespace
{

struct FootprintResource
{
    std::string            uri;
    std::list<std::string> model_uris;
};

struct DesignBlockResource
{
};


} // namespace


HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceDesignBlock( const HANDLER_CONTEXT<PlaceDesignBlock>& aCtx )
{
    if( !aCtx.Request.has_design_block_resource() )
        return {};

    return {};
}

HANDLER_RESULT<Empty> API_HANDLER_SCH::handlePlaceSymbol( const HANDLER_CONTEXT<PlaceSymbol>& aCtx )
{
    if( !aCtx.Request.has_component_resource() || !aCtx.Request.component_resource().has_symbol_resource()
        || aCtx.Request.component_resource().symbol_resource().uri().empty() )
        return {};


    const auto                   symbol_uri = aCtx.Request.component_resource().symbol_resource().uri();
    std::list<FootprintResource> footprint_resources;


    for( const auto& it : aCtx.Request.component_resource().footprint_resources() )
    {
        FootprintResource footprint_resource;
        footprint_resource.uri = it.uri();
        for( const auto& model : it.models() )
        {
            footprint_resource.model_uris.push_back( model.uri() );
        }
        footprint_resources.push_back( footprint_resource );
    }


    return {};
}