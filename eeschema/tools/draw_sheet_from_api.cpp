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

#include "sch_sheet_path.h"
#include <memory>

#include <kiplatform/ui.h>
#include <project_sch.h>
#include <tools/sch_drawing_tools.h>
#include <tools/sch_line_wire_bus_tool.h>
#include <tools/sch_selection_tool.h>
#include <tools/ee_grid_helper.h>
#include <tools/rule_area_create_helper.h>
#include <gal/graphics_abstraction_layer.h>
#include <design_block_lib_table.h>
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
#include <sch_textbox.h>
#include <sch_table.h>
#include <sch_tablecell.h>
#include <sch_sheet.h>
#include <sch_sheet_pin.h>
#include <sch_label.h>
#include <sch_bitmap.h>
#include <schematic.h>
#include <sch_commit.h>
#include <scoped_set_reset.h>
#include <symbol_library.h>
#include <eeschema_settings.h>
#include <string_utils.h>
#include <wildcards_and_files_ext.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>


int SCH_DRAWING_TOOLS::DrawSheetFromAPI( const TOOL_EVENT& aEvent )
{
    SCH_SHEET* sheet = nullptr;
    wxString   filename;


    wxString* ptr = aEvent.Parameter<wxString*>();
    wxCHECK( ptr, 0 );

    // We own the string if we're importing a sheet
    filename = *ptr;
    delete ptr;

    if( !wxFileExists( filename ) )
    {
        wxMessageBox( wxString::Format( _( "File '%s' does not exist." ), filename ) );
        return 0;
    }

    if( m_inDrawingTool )
        return 0;

    REENTRANCY_GUARD guard( &m_inDrawingTool );

    EESCHEMA_SETTINGS*    cfg = m_frame->eeconfig();
    SCHEMATIC_SETTINGS&   schSettings = m_frame->Schematic().Settings();
    KIGFX::VIEW_CONTROLS* controls = getViewControls();
    EE_GRID_HELPER        grid( m_toolMgr );
    VECTOR2I              cursorPos;

    m_toolMgr->RunAction( SCH_ACTIONS::clearSelection );

    m_frame->PushTool( aEvent );

    auto setCursor = [&]()
    {
        m_frame->GetCanvas()->SetCurrentCursor( KICURSOR::MOVING );
    };

    auto cleanup = [&]()
    {
        m_toolMgr->RunAction( SCH_ACTIONS::clearSelection );
        m_view->ClearPreview();
        delete sheet;
        sheet = nullptr;
    };

    Activate();

    // Must be done after Activate() so that it gets set into the correct context
    getViewControls()->ShowCursor( true );

    // Set initial cursor
    setCursor();

    // Main loop: keep receiving events
    while( TOOL_EVENT* evt = Wait() )
    {
        setCursor();
        grid.SetSnap( !evt->Modifier( MD_SHIFT ) );
        grid.SetUseGrid( getView()->GetGAL()->GetGridSnapping() && !evt->DisableGridSnapping() );

        cursorPos = grid.Align( controls->GetMousePosition(), GRID_HELPER_GRIDS::GRID_GRAPHICS );
        controls->ForceCursorPosition( true, cursorPos );

        // The tool hotkey is interpreted as a click when drawing
        bool isSyntheticClick = sheet && evt->IsActivate() && evt->HasPosition() && evt->Matches( aEvent );

        if( evt->IsCancelInteractive() || ( sheet && evt->IsAction( &ACTIONS::undo ) ) )
        {
            m_frame->GetInfoBar()->Dismiss();

            if( sheet )
            {
                cleanup();
            }
            else
            {
                m_frame->PopTool( aEvent );
                break;
            }
        }
        else if( evt->IsActivate() && !isSyntheticClick )
        {
            if( sheet && evt->IsMoveTool() )
            {
                // we're already drawing our own item; ignore the move tool
                evt->SetPassEvent( false );
                continue;
            }

            if( sheet )
            {
                m_frame->ShowInfoBarMsg( _( "Press <ESC> to cancel sheet creation." ) );
                evt->SetPassEvent( false );
                continue;
            }

            if( evt->IsPointEditor() )
            {
                // don't exit (the point editor runs in the background)
            }
            else if( evt->IsMoveTool() )
            {
                // leave ourselves on the stack so we come back after the move
                break;
            }
            else
            {
                m_frame->PopTool( aEvent );
                break;
            }
        }
        else if( !sheet && ( evt->IsClick( BUT_LEFT ) || evt->IsDblClick( BUT_LEFT ) ) )
        {
            SCH_SELECTION& selection = m_selectionTool->GetSelection();

            if( selection.Size() == 1 && selection.Front()->Type() == SCH_SHEET_T
                && selection.Front()->GetBoundingBox().Contains( cursorPos ) )
            {
                if( evt->IsClick( BUT_LEFT ) )
                {
                    // sheet already selected
                    continue;
                }
                else if( evt->IsDblClick( BUT_LEFT ) )
                {
                    m_toolMgr->PostAction( SCH_ACTIONS::enterSheet );
                    m_frame->PopTool( aEvent );
                    break;
                }
            }

            m_toolMgr->RunAction( SCH_ACTIONS::clearSelection );

            sheet = new SCH_SHEET( m_frame->GetCurrentSheet().Last(), cursorPos );
            sheet->SetScreen( nullptr );

            wxFileName fn( filename );

            sheet->GetFields()[SHEETNAME].SetText( fn.GetName() );
            sheet->GetFields()[SHEETFILENAME].SetText( fn.GetName() + wxT( "." )
                                                       + FILEEXT::KiCadSchematicFileExtension );

            sheet->SetFlags( IS_NEW | IS_MOVING );
            sheet->SetBorderWidth( schIUScale.MilsToIU( cfg->m_Drawing.default_line_thickness ) );
            sheet->SetBorderColor( cfg->m_Drawing.default_sheet_border_color );
            sheet->SetBackgroundColor( cfg->m_Drawing.default_sheet_background_color );
            sizeSheet( sheet, cursorPos );

            SCH_SHEET_LIST hierarchy = m_frame->Schematic().Hierarchy();
            SCH_SHEET_PATH instance = m_frame->GetCurrentSheet();
            instance.push_back( sheet );
            wxString pageNumber;

            // Don't try to be too clever when assigning the next availabe page number.  Just use
            // the number of sheets plus one.
            pageNumber.Printf( wxT( "%d" ), static_cast<int>( hierarchy.size() ) + 1 );
            instance.SetPageNumber( pageNumber );

            m_view->ClearPreview();
            m_view->AddToPreview( sheet->Clone() );
        }
        else if( sheet
                 && ( evt->IsClick( BUT_LEFT ) || evt->IsDblClick( BUT_LEFT ) || isSyntheticClick
                      || evt->IsAction( &ACTIONS::finishInteractive ) ) )
        {
            getViewControls()->SetAutoPan( false );
            getViewControls()->CaptureCursor( false );

            m_frame->EditSheetProperties( static_cast<SCH_SHEET*>( sheet ), &m_frame->GetCurrentSheet(), nullptr,
                                          nullptr, nullptr, &filename, true );
            m_view->ClearPreview();

            sheet->AutoplaceFields( m_frame->GetScreen(), AUTOPLACE_AUTO );

            // Use the commit we were provided or make our own
            SCH_COMMIT  tempCommit = SCH_COMMIT( m_toolMgr );
            SCH_COMMIT& c = evt->Commit() ? *( (SCH_COMMIT*) evt->Commit() ) : tempCommit;

            // We need to manually add the sheet to the screen otherwise annotation will not be able to find
            // the sheet and its symbols to annotate.
            m_frame->AddToScreen( sheet );
            c.Added( sheet, m_frame->GetScreen() );

            // Annotation will remove this from selection, but we add it back later
            m_selectionTool->AddItemToSel( sheet );

            NULL_REPORTER reporter;
            m_frame->AnnotateSymbols( &c, ANNOTATE_SELECTION, ANNOTATE_ORDER_T::SORT_BY_X_POSITION,
                                      ANNOTATE_ALGO_T::INCREMENTAL_BY_REF, true /* recursive */,
                                      schSettings.m_AnnotateStartNum, true, false, reporter );
            c.Push( "API Place Sheet" );

            m_selectionTool->AddItemToSel( sheet );
            sheet = nullptr;
            m_frame->PopTool( aEvent );
            break;
        }
        else if( evt->IsAction( &ACTIONS::duplicate ) || evt->IsAction( &SCH_ACTIONS::repeatDrawItem ) )
        {
            if( sheet )
            {
                // This doesn't really make sense; we'll just end up dragging a stack of
                // objects so we ignore the duplicate and just carry on.
                wxBell();
                continue;
            }

            // Exit.  The duplicate will run in its own loop.
            m_frame->PopTool( aEvent );
            evt->SetPassEvent();
            break;
        }
        else if( sheet && ( evt->IsAction( &ACTIONS::refreshPreview ) || evt->IsMotion() ) )
        {
            sizeSheet( sheet, cursorPos );
            m_view->ClearPreview();
            m_view->AddToPreview( sheet->Clone() );
            m_frame->SetMsgPanel( sheet );
        }
        else if( evt->IsClick( BUT_RIGHT ) )
        {
            // Warp after context menu only if dragging...
            if( !sheet )
                m_toolMgr->VetoContextMenuMouseWarp();

            m_menu->ShowContextMenu( m_selectionTool->GetSelection() );
        }
        else if( sheet && evt->IsAction( &ACTIONS::redo ) )
        {
            wxBell();
        }
        else
        {
            evt->SetPassEvent();
        }

        // Enable autopanning and cursor capture only when there is a sheet to be placed
        getViewControls()->SetAutoPan( sheet != nullptr );
        getViewControls()->CaptureCursor( sheet != nullptr );
    }

    getViewControls()->SetAutoPan( false );
    getViewControls()->CaptureCursor( false );
    m_frame->GetCanvas()->SetCurrentCursor( KICURSOR::ARROW );

    return 0;
}
