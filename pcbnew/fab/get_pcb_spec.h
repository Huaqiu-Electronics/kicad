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

#ifndef GET_PCB_SPEC_H
#define GET_PCB_SPEC_H

#include <pad.h>
#include <optional>
#include <pcb_edit_frame.h>
#include <board.h>
#include <board_design_settings.h>
#include <pcb_track.h>

template <>
FAB_PCB_SPECIFICATIONS FAB_INFO_GETTER_PCB::GetPCBSpecifications()
{
    BOARD_DESIGN_SETTINGS& dsnSettings = self()->GetDesignSettings();
    BOARD_STACKUP&         stackup = dsnSettings.GetStackupDescriptor();
    stackup.SynchronizeWithBoard( &dsnSettings );

    double                boardThickness = stackup.BuildBoardThicknessFromStackup();
    std::optional<double> outer_copper_thickness{}, inner_copper_thickness{};

    for( const auto& layer : stackup.GetList() )
    {
        if( layer->GetBrdLayerId() == F_Cu || layer->GetBrdLayerId() == B_Cu )
        {
            outer_copper_thickness = std::max( layer->GetThickness() / pcbIUScale.IU_PER_MM,
                                               outer_copper_thickness ? *outer_copper_thickness : 0 );
        }
        else if( layer->GetType() == BOARD_STACKUP_ITEM_TYPE::BS_ITEM_TYPE_COPPER )
        {
            inner_copper_thickness = std::max( layer->GetThickness() / pcbIUScale.IU_PER_MM,
                                               inner_copper_thickness ? *inner_copper_thickness : 0 );
        }
    }


    const auto board = self()->GetBoard();
    const auto layers = board->GetCopperLayerCount();
    const auto height = board->GetBoundingBox().GetHeight();
    const auto width = board->GetBoundingBox().GetWidth();

    std::optional<int> minTraceWidth{};
    auto               minHoleSize =
            std::min( board->GetDesignSettings().m_MinThroughDrill, board->GetDesignSettings().m_ViasMinSize );


    for( const auto& track : board->Tracks() )
    {
        switch( track->Type() )
        {
        case PCB_VIA_T: minHoleSize = std::min( minHoleSize, static_cast<PCB_VIA*>( track )->GetDrillValue() ); break;
        case PCB_TRACE_T:
        case PCB_ARC_T:
        {
            if( !minTraceWidth )
            {
                minTraceWidth = track->GetWidth();
                break;
            }

            minTraceWidth = std::min( *minTraceWidth, track->GetWidth() );
            break;
        }
        default: break;
        }
    }

    for( const auto& pad : board->GetPads() )
    {
        minHoleSize = std::min( minHoleSize, pad->GetDrillSize().x );
    }


    return {
        layers,
        static_cast<double>( height ) / pcbIUScale.IU_PER_MM,
        static_cast<double>( width ) / pcbIUScale.IU_PER_MM,
        static_cast<double>( minTraceWidth ? *minTraceWidth : 0 ) / pcbIUScale.IU_PER_MM,
        static_cast<double>( minHoleSize ) / pcbIUScale.IU_PER_MM,
        static_cast<double>( boardThickness ) / pcbIUScale.IU_PER_MM,
        inner_copper_thickness,
        outer_copper_thickness,
    };
}


#endif
