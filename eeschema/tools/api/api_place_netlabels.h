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

#ifndef API_PLACE_NETLABELS_H
#define API_PLACE_NETLABELS_H

#include "sch_sheet_path.h"

#include <kiplatform/ui.h>
#include <project_sch.h>
#include <tools/sch_drawing_tools.h>
#include <tools/sch_line_wire_bus_tool.h>
#include <tools/sch_selection_tool.h>
#include <tools/ee_grid_helper.h>
#include <sch_actions.h>
#include <sch_tool_utils.h>
#include <sch_edit_frame.h>
#include <pgm_base.h>
#include <eeschema_id.h>
#include <confirm.h>
#include <view/view_controls.h>
#include <view/view.h>
#include <sch_symbol.h>
#include <sch_no_connect.h>
#include <sch_line.h>
#include <sch_junction.h>
#include <sch_bus_entry.h>
#include <sch_rule_area.h>
#include <sch_text.h>
#include <sch_sheet.h>
#include <sch_sheet_pin.h>
#include <sch_label.h>
#include <schematic.h>
#include <sch_commit.h>
#include <scoped_set_reset.h>
#include <eeschema_settings.h>
#include <api/api_place_netlabel_params.h>
#include <string_utils.h>
#include <wx/string.h>

static bool getPinConnectionPosition( SCH_SHEET_PATH& aSheet, const std::string& aDesignator, int aPinNum,
                                      VECTOR2I* aPosition, SCH_SYMBOL** aSymbolOut )
{
    SCH_SCREEN* screen = aSheet.LastScreen();
    for( SCH_ITEM* item : screen->Items().OfType( SCH_SYMBOL_T ) )
    {
        SCH_SYMBOL* sym = static_cast<SCH_SYMBOL*>( item );

        if( sym->GetRef( &aSheet, true ).ToStdString() == aDesignator )
        {
            SCH_PIN* pin = sym->GetPin( wxString( wxString::Format( wxT( "%d" ), aPinNum ) ) );

            if( pin )
            {
                *aPosition = pin->GetPosition();
                if( aSymbolOut )
                    *aSymbolOut = sym;
                return true;
            }
        }
    }
    return false;
}

int SCH_DRAWING_TOOLS::apiPlaceNetlabels( const TOOL_EVENT& aEvent )
{
    const auto     params = aEvent.Parameter<API_PLACE_NETLABEL_PARAMS>();
    SCH_SHEET_PATH current = m_frame->GetCurrentSheet();
    SCH_SCREEN*    screen = current.LastScreen();

    SCH_COMMIT commit( m_toolMgr );

    int placed_count = 0;

    for( const API_PLACE_NETLABEL_PIN& pin_data : params.pins )
    {
        VECTOR2I    pin_pos;
        SCH_SYMBOL* symbol = nullptr;

        if( !getPinConnectionPosition( current, pin_data.designator, pin_data.pin_num, &pin_pos, &symbol ) )
        {
            continue;
        }

        SCH_LABEL* newLabel = new SCH_LABEL();
        newLabel->SetParent(symbol);

        newLabel->SetText( params.net_name );
        newLabel->SetPosition( pin_pos );
        newLabel->SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
        newLabel->SetVertJustify( GR_TEXT_V_ALIGN_CENTER );
        newLabel->SetTextAngle( ANGLE_0 );
        m_frame->AddToScreen( newLabel, screen );

        commit.Added( newLabel, screen );
        placed_count++;
    }

    if( placed_count > 0 )
    {
        commit.Push( wxString::Format( _( "Place %d Net Labels via API" ), placed_count ) );
    }

    return 0;
}

#endif
