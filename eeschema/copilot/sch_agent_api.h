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

#ifndef SCH_AGENT_API_H
#define SCH_AGENT_API_H

#include <confirm.h>
#include <cstddef>
#include <memory>
#include <sch_editor_control.h>
#include <tool/tool_manager.h>
#include <tools/sch_editor_control.h>
#include <sch_edit_frame.h>
#include <fstream>
#include <sstream>


void SCH_EDIT_FRAME::HighlightSymbol( std::string const& aDesignator )
{
    wxString            part_ref{ aDesignator };
    SCH_EDITOR_CONTROL* editor = m_toolManager->GetTool<SCH_EDITOR_CONTROL>();
    editor->FindSymbolAndItem( nullptr, &part_ref, true, HIGHLIGHT_SYMBOL, {} );
}

wxString SCH_EDIT_FRAME::DumpSelectionToDesignBlock()
{
    // Get all selected items
    SCH_SELECTION selection = m_toolManager->GetTool<SCH_SELECTION_TOOL>()->GetSelection();

    if( selection.Empty() )
    {
        DisplayErrorMessage( this, _( "Please select some items to save as a design block." ) );
        return {};
    }

    // Just block all attempts to create design blocks with nested sheets at this point
    if( selection.HasType( SCH_SHEET_T ) )
    {
        DisplayErrorMessage( this, _( "Design blocks with nested sheets are not supported." ) );

        return {};
    }


    // Create a temporary screen
    SCH_SCREEN* tempScreen = new SCH_SCREEN( m_schematic );

    // Copy the selected items to the temporary screen
    for( EDA_ITEM* item : selection )
    {
        EDA_ITEM* copy = item->Clone();
        tempScreen->Append( static_cast<SCH_ITEM*>( copy ) );
    }

    // Create a sheet for the temporary screen
    SCH_SHEET* tempSheet = new SCH_SHEET( m_schematic );
    tempSheet->SetScreen( tempScreen );

    std::shared_ptr<nullptr_t> clean_sheet( nullptr,
                                            [&]( nullptr_t )
                                            {
                                                delete tempSheet;
                                            } );


    // Save a temporary copy of the schematic file, as the plugin is just going to move it
    wxString                   tempFile = wxFileName::CreateTempFileName( "design_block" );
    std::shared_ptr<nullptr_t> clean_file( nullptr,
                                           [&]( nullptr_t )
                                           {
                                               wxRemoveFile( tempFile );
                                           } );


    if( !saveSchematicFile( tempSheet, tempFile ) )
    {
        DisplayErrorMessage( this, _( "Error saving temporary schematic file to create design block." ) );
        return {};
    }

    std::ifstream     file( tempFile.ToStdString() );
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void SCH_EDIT_FRAME::RunMainStack( std::function<void()> aFunc )()
{
    m_toolManager->GetTool<SCH_EDITOR_CONTROL>()->RunMainStack( aFunc );
}


#endif
