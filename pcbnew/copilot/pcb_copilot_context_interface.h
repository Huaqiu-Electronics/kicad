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

#ifndef PCB_COPILOT_CONTEXT_INTERFACE_H
#define PCB_COPILOT_CONTEXT_INTERFACE_H

#include "tool/tool_manager.h"
#include "tools/pcb_selection_tool.h"
#include "pcb_items_jsonify_helper.h"

#include <copilot_json_helper.h>
#include <webview/webview_container.h>
#include <board_design_settings.h>
#include <string_utils.h>
#include <algorithm>
#include <pcb_edit_frame.h>
#include <context/pcb/pcb_copilot_global_context.h>
#include <context/pcb/details/board_ir.h>
#include <string>
#include <board.h>
#include <footprint.h>


template <>
KICAD_COPILOT_EDITORS PCB_COPILOT_PANEL_CONTAINER::editor_type() const
{
    return KICAD_COPILOT_EDITORS::pcb;
}


template <>
void PCB_COPILOT_PANEL_CONTAINER::UpdateCopilotContextCache()
{
    if( m_copilotContextCache.is_newest )
        return;

    m_copilotContextCache.uuid = KIID().AsStdString();
    m_copilotContextCache.is_newest = true;
    auto board = self()->GetBoard();
    m_copilotContextCache.designators = ([board](){

        std::vector<std::string> designators;

        for(const auto& fp : board->Footprints())        
            designators.push_back(UnescapeString(fp->Reference().GetText()).ToStdString());        
        std::sort(designators.begin(), designators.end(),[&](const std::string& a, const std::string& b){
            return StrNumCmp(a, b) < 0;
        });

        designators.erase( std::unique( designators.begin(), designators.end() ),
                           designators.end() );

        return designators;

    })();
}


template <>
std::string PCB_COPILOT_PANEL_CONTAINER::GetJsonifySelection() const
{
    nlohmann::json selection = self()->GetSelection();
    RemoveEmptyObjectsOrArrays( selection );
    return selection.dump();
}


copilot::BOARD_SELECTIONS PCB_EDIT_FRAME::GetSelection() const
{
    return jsonify_pcb_items( GetToolManager()->GetTool<PCB_SELECTION_TOOL>()->GetSelection() );
}


#endif
