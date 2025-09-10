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
#include "pcb_field.h"
#include "template_fieldnames.h"
#include <optional>
#include <project.h>
#include <api/api_fmt_fp_model_path.h>
#include <api/api_design_resource.h>
#include <api/api_design_content.h>
#include <eda_base_frame.h>
#include <project_pcb.h>
#include <wx/log.h>
#include <fp_lib_table.h>

#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <pcb_io/pcb_io_mgr.h>
#include <footprint.h>
#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <wx/log.h>
#include <io/kicad/kicad_io_utils.h>
#include <api/component_field_name.h>


static FOOTPRINT* UpdateFpAttributes( wxString const& fpFilePath, std::optional<wxString> const& modelPath,
                                      std::list<COMPONENT_CONTENT::Attribute> const& attributes )
{
    PCB_IO_KICAD_SEXPR pi;


    static const std::map<std ::string, MANDATORY_FIELD_T> name_field_t_mapping = {
        { kDATASHEET, MANDATORY_FIELD_T::DATASHEET_FIELD },
        { kDESCRIPTION, MANDATORY_FIELD_T::DESCRIPTION_FIELD },
        { kVALUE, MANDATORY_FIELD_T::VALUE_FIELD }

    };

    if( pi.CanReadFootprint( fpFilePath ) )
    {
        wxString footprintNameOut;

        auto footprint = pi.ImportFootprint( fpFilePath, footprintNameOut );

        if( !footprint )
            return {};

        footprint->Models().clear();

        if( modelPath )
        {
            const auto model = new FP_3DMODEL();
            model->m_Filename = *modelPath;
            footprint->Add3DModel( model );
        }

        int usr_filed = MANDATORY_FIELD_T::MANDATORY_FIELD_COUNT;

        for( const auto& [name, value] : attributes )
        {
            if( auto f_t = name_field_t_mapping.find( name ); f_t != name_field_t_mapping.end() )
            {
                const auto f_num = f_t->second;
                if( auto old_field = footprint->GetFieldById( f_num ) )
                {
                    old_field->SetText( value );
                }
                else
                {
                    PCB_FIELD field( footprint, f_num, name );
                    field.SetText( value );
                    footprint->AddField( field );
                }

                continue;
            }

            PCB_FIELD field( footprint, ++usr_filed, name );
            field.SetLayer( PCB_LAYER_ID::F_Fab );
            field.SetText( value );
            field.SetVisible( false );
            footprint->AddField( field );
        }


        pi.Format( footprint );

        FILE* fp = wxFopen( fpFilePath, wxT( "wt" ) );

        if( fp == nullptr )
        {
            wxLogError( wxString::Format( _( "Insufficient permissions to write file '%s'." ), fpFilePath ) );
            return {};
        }

        std::string prettyData = pi.GetStringOutput( false );
        KICAD_FORMAT::Prettify( prettyData, true );

        fprintf( fp, "%s", prettyData.c_str() );
        fclose( fp );

        return footprint;
    }

    return {};
}


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
        std::optional<wxString> model_path;

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

        const auto fp_file_path = *GetProjectResourceSavePath( PRETTY_DIR, fpc.footprint.filename );

        if( auto fp = UpdateFpAttributes( fp_file_path, model_path, aComponent.attributes ) )
        {
            if( !fp_loaded )
            {
                fp_loaded = fp;
            }
            else
            {
                delete fp;
            }
        }
    }

    UpdateLibTable( API_LIB_FP_TABLE, GetAPIFootprintPrettyName(), GetProjectApiFpUri() );


    if( !fp_loaded )
    {
        wxLogError( "Failed to load fp " );
        return {};
    }

    return fp_loaded.value();
}
