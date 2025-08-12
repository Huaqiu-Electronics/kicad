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


#include <pcb_edit_frame.h>
#include <board.h>
#include <board_design_settings.h>
#include <pcb_track.h>

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


#endif
