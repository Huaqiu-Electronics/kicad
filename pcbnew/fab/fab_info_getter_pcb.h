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

#ifndef FAB_INFO_GETTER_PCB_H
#define FAB_INFO_GETTER_PCB_H

#include <dialog_gen_footprint_position.h>
#include <dialog_plot.h>
#include <core/typeinfo.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pcb_edit_frame.h>
#include <board.h>
#include <board_design_settings.h>
#include <base_units.h>
#include <vector>
#include <tools/pcb_actions.h>


template <>
FAB_PCB_SPECIFICATIONS FAB_INFO_GETTER_PCB::GetPCBSpecifications()
{
    const auto board = self()->GetBoard();
    const auto layers = board->GetCopperLayerCount();
    const auto height = board->GetBoundingBox().GetHeight();
    const auto width = board->GetBoundingBox().GetWidth();

    auto       minTraceWidth = board->GetDesignSettings().m_TrackMinWidth;
    const auto minTraceClearance = board->GetDesignSettings().m_MinClearance;
    auto       minHoleSize =
            std::min( board->GetDesignSettings().m_MinThroughDrill, board->GetDesignSettings().m_ViasMinSize );


    for( const auto& track : board->Tracks() )
    {
        switch( track->Type() )
        {
        case PCB_VIA_T: minHoleSize = std::min( minHoleSize, static_cast<PCB_VIA*>( track )->GetDrillValue() ); break;
        case PCB_TRACE_T:
        case PCB_ARC_T: minTraceWidth = std::min( minTraceWidth, track->GetWidth() ); break;
        default: break;
        }
    }

    return { layers,
             static_cast<double>( height ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( width ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minTraceClearance ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minTraceWidth ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minHoleSize ) / pcbIUScale.IU_PER_MM };
}

template <>
GERBER_FILENAME_CONTENT FAB_INFO_GETTER_PCB::GetGerber()
{
    auto            editor = static_cast<PCB_EDIT_FRAME*>( this );
    PCB_PLOT_PARAMS plotSettings = editor->GetPlotSettings();
    plotSettings.SetFormat( PLOT_FORMAT::GERBER );
    editor->SetPlotSettings( plotSettings );
    DIALOG_PLOT dlg( self() );
    dlg.ShowQuasiModal();
    return {};
}

template <>
std::vector<FP_POSITION_LIST> FAB_INFO_GETTER_PCB::GetComponentPosition()
{
    DIALOG_GEN_FOOTPRINT_POSITION dlg( self() );
    dlg.ShowModal();
    return {};
}

template <>
std::vector<FAB_BOM_ITEM> FAB_INFO_GETTER_PCB::GetBOM()
{
    std::vector<FAB_BOM_ITEM> items;

    const auto board = static_cast<const PCB_EDIT_FRAME*>( this )->GetBoard();

    struct BOM_ENTRY
    {
        std::vector<std::string> m_Ref;
        std::string              m_Val;
        LIB_ID                   m_FPID;
        int                      m_Id;
        int                      m_Count;
    };


    using BOM_ENTRY_LIST = std::vector<BOM_ENTRY*>;

    // Build list
    BOM_ENTRY_LIST list;
    int            i = 1;

    for( FOOTPRINT* footprint : board->Footprints() )
    {
        if( footprint->GetAttributes() & FP_EXCLUDE_FROM_BOM )
            continue;

        bool valExist = false;

        // try to find component in existing list
        for( auto iter = list.begin(); iter != list.end(); ++iter )
        {
            BOM_ENTRY* curEntry = *iter;

            if( curEntry->m_Val == footprint->GetValue() && curEntry->m_FPID == footprint->GetFPID() )
            {
                curEntry->m_Ref.push_back( ( footprint->Reference().GetShownText( false ) ).ToStdString() );
                curEntry->m_Count++;

                valExist = true;
                break;
            }
        }

        // If component does not exist yet, create new one and append it to the list.
        if( valExist == false )
        {
            BOM_ENTRY* newEntry = new BOM_ENTRY();
            newEntry->m_Id = i++;
            newEntry->m_Val = footprint->Value().GetShownText( false );
            newEntry->m_Ref.push_back( footprint->Reference().GetShownText( false ).ToStdString() );
            newEntry->m_FPID = footprint->GetFPID();
            newEntry->m_Count = 1;
            list.push_back( newEntry );
        }
    }

    // Print list. Also delete temporary created objects.
    for( size_t ii = list.size(); ii > 0; ii-- )
    {
        BOM_ENTRY* curEntry = *list.begin();
        // TODO : Add MPN

        items.push_back( {

                curEntry->m_Ref,
                From_UTF8( curEntry->m_FPID.GetLibItemName().c_str() ).ToStdString(),
                curEntry->m_Count,
                curEntry->m_Val,
                {}

        } );

        delete curEntry;
    }

    return items;
}


#endif
