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


    auto libTable = PROJECT_PCB::PcbFootprintLibs( &m_frame.Prj() );

    if( !libTable )
    {
        wxLogError( "Failed to get footprint library table" );
        return {};
    }

    auto fp = libTable->FootprintLoad( GetAPIFootprintPrettyName(), GetProjectApiFpUri(), true );

    if( !fp )
    {
        wxLogError( "Failed to load fp for " );
        return {};
    }

    return fp;
}


LIB_ID API_PCB_DESIGN_RESOURCE_MGR::WriteFootprintFields( std::string const& aFileName, auto const& aFields,
                                                          auto const& aFootprintName )
{
    return {};
}
