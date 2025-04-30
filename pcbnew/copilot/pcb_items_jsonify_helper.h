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

#ifndef PCB_ITEMS_JSONIFY_HELPER_H
#define PCB_ITEMS_JSONIFY_HELPER_H


#include "pad.h"
#include "tools/pcb_selection.h"
#include <board_design_settings.h>
#include <string_utils.h>
#include <pcb_edit_frame.h>
#include <context/pcb/pcb_copilot_global_context.h>
#include <context/pcb/details/board_ir.h>
#include <string>
#include <board.h>
#include <footprint.h>


inline auto jsonify_pcb_items( const PCB_SELECTION& aSelection ) -> copilot::BOARD
{
    copilot::BOARD board;

    for( const auto& selection : aSelection )
    {
        switch( selection->Type() )
        {
        case PCB_FOOTPRINT_T:
        {
            auto               footprint = static_cast<FOOTPRINT*>( selection );
            copilot::FOOTPRINT f;
            f.ref = footprint->GetReference();
            f.value = footprint->GetValue();
            f.footprint = footprint->GetFPID().GetLibItemName();
            f.x = footprint->GetPosition().x;
            f.y = footprint->GetPosition().y;
            f.rotation = footprint->GetOrientationDegrees();

            for( const PAD* pad : footprint->Pads() )
            {
                copilot::PAD p;
                p.pin = pad->GetNumber();
                p.net = pad->GetNetname();
                p.x = pad->GetPosition().x;
                p.y = pad->GetPosition().y;
                p.rotation = pad->GetOrientationDegrees();
                f.pads.push_back( p );
            }

            board.footprints.push_back( f );
            break;
        }

        case PCB_TEXT_T:
        {
            break;
        }
        case PCB_TRACE_T:
        case PCB_SHAPE_T:
        {
            break;
        }


        case PCB_ZONE_T:
        {
            break;
        }

        case PCB_PAD_T: break;

        case PCB_REFERENCE_IMAGE_T: break;

        case PCB_FIELD_T: break;

        case PCB_GENERATOR_T: break;

        case PCB_TEXTBOX_T: break;

        case PCB_TABLE_T:
        case PCB_TABLECELL_T: break;

        case PCB_VIA_T: break;

        case PCB_ARC_T: break;

        case PCB_MARKER_T: break;

        case PCB_DIMENSION_T:
        case PCB_DIM_ALIGNED_T:
        case PCB_DIM_LEADER_T:
        case PCB_DIM_CENTER_T:
        case PCB_DIM_RADIAL_T:
        case PCB_DIM_ORTHOGONAL_T: break;

        case PCB_TARGET_T: break;

        case PCB_ITEM_LIST_T: break;

        case PCB_NETINFO_T: break;

        case PCB_GROUP_T: break;

        default:
            // Unknown or unhandled type
            break;
        }
    }

    return board;
}


#endif
