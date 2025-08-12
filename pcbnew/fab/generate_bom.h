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

#ifndef GENERATE_BOM_H
#define GENERATE_BOM_H

#include <board.h>
#include <pcbnew_settings.h>
#include <dialog_gen_footprint_position.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pcb_edit_frame.h>
#include <exporters/gendrill_Excellon_writer.h>


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
    for( auto& curEntry : list )
    {
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
