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

#include <cmath>
#include <sch_io/sch_io.h>
#include "sch_sheet_path.h"
#include "tool/grid_helper.h"
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
#include <vector>
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


struct SidePadding
{
    int l_side = 0;
    int r_side = 0;
};

struct BlockLabelPadding
{
    SidePadding internal;
    SidePadding outer;
    int         total_pin_num = 0;
};

inline auto get_sheet_symbol_lb_padding( const std::vector<SCH_HIERLABEL*>& pins, double FONT_SIZE )
{
    BlockLabelPadding factor;
    factor.total_pin_num = static_cast<int>( pins.size() );

    int port_count = static_cast<int>( pins.size() );
    int h_count = static_cast<int>( std::ceil( port_count / 2.0 ) );

    for( int i = 0; i < port_count; ++i )
    {
        bool        is_left = i < h_count;
        const auto& cur_pin = pins[i];
        int         name_len = static_cast<int>( cur_pin->GetShownText( true ).length() );

        if( is_left )
            factor.internal.l_side = std::max( factor.internal.l_side, name_len );
        else
            factor.internal.r_side = std::max( factor.internal.r_side, name_len );
    }

    BlockLabelPadding result;
    result.internal.l_side = factor.internal.l_side * FONT_SIZE;
    result.internal.r_side = factor.internal.r_side * FONT_SIZE;
    result.outer.l_side = factor.outer.l_side * FONT_SIZE;
    result.outer.r_side = factor.outer.r_side * FONT_SIZE;
    result.total_pin_num = factor.total_pin_num;
    return result;
}


inline auto GetSheetFilledWithPins( const VECTOR2I& aPos, SCH_EDIT_FRAME* aFrame, wxString const& aFileName )
{
    const auto                 schematic = &aFrame->Schematic();
    std::unique_ptr<SCH_SHEET> tmpSheet = std::make_unique<SCH_SHEET>( schematic );

    auto screen = ([&] ()-> const SCH_SCREEN* {
        wxFileName                 fn( aFileName );
        const auto full_fp = fn.GetFullPath();


        for( auto s : schematic->GetSchematicsSharedByMultipleProjects()) {
            if(s->GetFileName() == full_fp)            
                return s;            
        }

        SCH_IO_MGR::SCH_FILE_T schFileType = SCH_IO_MGR::GuessPluginTypeFromSchPath( aFileName );
        if( schFileType == SCH_IO_MGR::SCH_FILE_UNKNOWN )
            schFileType = SCH_IO_MGR::SCH_KICAD;

        IO_RELEASER<SCH_IO> pi( SCH_IO_MGR::FindPlugin( schFileType ) );


        tmpSheet->GetFields()[SHEETNAME].SetText( fn.GetName() );
        tmpSheet->GetFields()[SHEETFILENAME].SetText( fn.GetName() + wxT( "." ) + FILEEXT::KiCadSchematicFileExtension );

        pi->LoadSchematicFile( full_fp, schematic, tmpSheet.get() );
        return tmpSheet->GetScreen();
    })();


    std::vector<SCH_HIERLABEL*> lbs;

    for( const auto& it : screen->Items() )
    {
        if( it->Type() == KICAD_T::SCH_HIER_LABEL_T )
        {
            auto lb = static_cast<SCH_HIERLABEL*>( it );
            lbs.push_back( lb );
        }
    }

    const auto pin_count = static_cast<int>( lbs.size() );
    constexpr auto GRID_SIZE = 2540; // 100 mils
    const auto BLOCK_PIN_TB_MARGIN = 8 * GRID_SIZE;
    const auto BLOCK_PIN_GAP = 16 * GRID_SIZE;
    const auto     FONT_SIZE = GRID_SIZE;
    const auto BLOCK_START_Y = aPos.y;
    const auto BLOCK_START_X = aPos.x;
    const auto INTERNAL_LABEL_MARGIN = FONT_SIZE;
    const int  h_count = std::ceil( pin_count / 2 );
    auto       paddings = get_sheet_symbol_lb_padding( lbs, FONT_SIZE );


    const int sheet_width =
            std::max<int>( schIUScale.MilsToIU( MIN_SHEET_WIDTH ),
                           paddings.internal.l_side + paddings.internal.r_side + INTERNAL_LABEL_MARGIN );

    const int sheet_height =
            std::max<int>( h_count * BLOCK_PIN_GAP + 2 * BLOCK_PIN_TB_MARGIN, schIUScale.MilsToIU( MIN_SHEET_HEIGHT ) );


    const auto aSheet = new SCH_SHEET( aFrame->GetCurrentSheet().Last(), aPos, VECTOR2I{ sheet_width, sheet_height } );


    for( auto i = 0; i < pin_count; i++ )
    {
        const auto is_left = i < h_count;
        const auto x = is_left ? BLOCK_START_X : BLOCK_START_X + sheet_width;
        const int  y = BLOCK_START_Y + BLOCK_PIN_GAP * abs( i < h_count ? i : i - h_count ) + BLOCK_PIN_TB_MARGIN;
        const auto rotation = is_left ? 180 : 0;
        const auto cur_lb = lbs[i];

        SCH_SHEET_PIN* newPin = new SCH_SHEET_PIN( aSheet, VECTOR2I{ x, y }, cur_lb->GetText() );
        newPin->SetShape( cur_lb->GetShape() );
        aSheet->AddPin( newPin );

        newPin->SetPosition( VECTOR2I{ x, y } );
        newPin->SetTextAngleDegrees( rotation );
        newPin->SetHorizJustify( is_left ? GR_TEXT_H_ALIGN_T::GR_TEXT_H_ALIGN_LEFT
                                         : GR_TEXT_H_ALIGN_T::GR_TEXT_H_ALIGN_RIGHT );
        newPin->SetSide( is_left ? SHEET_SIDE::LEFT : SHEET_SIDE::RIGHT );
    }

    return aSheet;
}


int SCH_DRAWING_TOOLS::DrawSheetFromAPI( const TOOL_EVENT& aEvent )
{
    SCH_SHEET* sheet = nullptr;
    bool       initialized = false;
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

        if( !initialized )
        {
            initialized = true;
            sheet = GetSheetFilledWithPins( cursorPos, m_frame, filename );

            sheet->SetScreen( nullptr );

            wxFileName fn( filename );

            sheet->GetFields()[SHEETNAME].SetText( fn.GetName() );
            sheet->GetFields()[SHEETFILENAME].SetText( fn.GetName() + wxT( "." )
                                                       + FILEEXT::KiCadSchematicFileExtension );

            sheet->SetFlags( IS_NEW | IS_MOVING );
            sheet->SetBorderWidth( schIUScale.MilsToIU( cfg->m_Drawing.default_line_thickness ) );
            sheet->SetBorderColor( cfg->m_Drawing.default_sheet_border_color );
            sheet->SetBackgroundColor( cfg->m_Drawing.default_sheet_background_color );

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


        // The tool hotkey is interpreted as a click when drawing
        bool isSyntheticClick = sheet && evt->IsActivate() && evt->HasPosition() && evt->Matches( aEvent );

        if( evt->IsCancelInteractive() || ( sheet && evt->IsAction( &ACTIONS::undo ) ) )
        {
            m_frame->GetInfoBar()->Dismiss();

            if( sheet )
            {
                cleanup();
            }

            m_frame->PopTool( aEvent );
            break;
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
            sheet->SetPosition( cursorPos );
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
