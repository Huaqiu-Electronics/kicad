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
#include "update_fp_model_path.h"
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
    {
        wxLogWarning( "Failed to save symbol %s to project", aComponent.symbol.filename );
    }

    std::optional<FOOTPRINT*> fp_loaded;

    auto libTable = PROJECT_PCB::PcbFootprintLibs( &m_frame.Prj() );

    if( !libTable )
    {
        wxLogError( "Failed to get footprint library table" );
        return {};
    }


    for( const auto& fpc : aComponent.footprints )
    {
        std::optional<std::string> model_path;

        for( const auto& model : fpc.models )
        {
            if( Save3dModelToProject( model, {} ) )
            {
                if( !model_path )
                {
                    model_path = FormatFootprintModelPath( model.filename );
                }
            }
        }

        if( !SaveFootprintContentToProject( fpc.footprint ) )
        {
            wxLogError( "Failed to save footprint %s to project", fpc.footprint.filename );
            continue;
        }

        if( model_path )
        {
            UpdateFpModelPath( *GetProjectResourceSavePath( PRETTY_DIR, fpc.footprint.filename ), *model_path );
        }


        const auto fp_name = RemoveFileExt( fpc.footprint.filename );
        const auto fp_t_nick_name = GetAPIFootprintPrettyName();


        UpdateLibTable( API_LIB_FP_TABLE, fp_t_nick_name, GetProjectApiFpUri() );

        if( !fp_loaded )
        {
            fp_loaded = libTable->FootprintLoad( fp_t_nick_name, fp_name, true );
        }
    }


    if( !fp_loaded )
    {
        wxLogError( "Failed to load fp for " );
        return {};
    }

    return fp_loaded.value();
}
