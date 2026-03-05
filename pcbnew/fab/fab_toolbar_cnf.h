/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2026 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2026 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef FAB_TOOLBAR_CNF_H
#define FAB_TOOLBAR_CNF_H

#include <tools/pcb_selection_tool.h>
#include <tool/ui/toolbar_configuration.h>
#include <tool/ui/toolbar_configuration.h>
#include <tool/actions.h>
#include <tools/pcb_actions.h>
inline auto GetFabToolbarConf( TOOLBAR_CONFIGURATION& aConfig )
{
    aConfig.AppendSeparator()
            .AppendAction( PCB_ACTIONS::placeOrderMenu )
            .WithContextMenu(
                    []( TOOL_MANAGER* aMgr ) -> std::unique_ptr<ACTION_MENU>
                    {
                        PCB_SELECTION_TOOL*          selTool = aMgr->GetTool<PCB_SELECTION_TOOL>();
                        std::unique_ptr<ACTION_MENU> menu = std::make_unique<ACTION_MENU>( false, selTool );

                        menu->Add( ACTIONS::placeOrderForPCB, ACTION_MENU::NORMAL );
                        menu->Add( ACTIONS::placeOrderForSMT, ACTION_MENU::NORMAL );
                        return menu;
                    } );
}


#endif
