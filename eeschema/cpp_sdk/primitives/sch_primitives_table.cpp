#include "sch_primitives_table.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_table.h>

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_TABLE::CreateTable(const VECTOR2I& position, int rows, int cols, std::string& aKID)
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC* schematic = nullptr;
    GetSchEditorFrame(frame);
    GetSchematic(schematic);

    if(nullptr == frame || nullptr == schematic)
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;

    SCH_TABLE*   table = nullptr;
    table  = new SCH_TABLE(0);
    table->SetColCount(1);
    SCH_TABLECELL*   tableCell = new SCH_TABLECELL();

    int defaultTextSize = schematic->Settings().m_DefaultTextSize;


    tableCell->SetTextSize(VECTOR2I{defaultTextSize, defaultTextSize});
    table->AddCell(tableCell);
    table->SetParent(schematic);
    table->SetFlags(IS_NEW);
    table->SetPosition(position);

    int aSize = schematic->Settings().m_ConnectionGridSize;

    VECTOR2I gridSize = {aSize, aSize};

    VECTOR2I cellSize{gridSize.x * 20, gridSize.y * 2};

    table->ClearCells();
    table->SetColCount( cols );

    for( int col = 0; col < cols; ++col )
        table->SetColWidth( col, cellSize.x );

    for( int row = 0; row < rows; ++row )
    {
        table->SetRowHeight( row, cellSize.y );
        for( int col = 0; col < cols; ++col )
        {
            SCH_TABLECELL* cell = new SCH_TABLECELL();
            defaultTextSize = schematic->Settings().m_DefaultTextSize;
            cell->SetTextSize( VECTOR2I( defaultTextSize, defaultTextSize ) );
            cell->SetPosition( position + VECTOR2I( col * cellSize.x, row * cellSize.y ) );
            cell->SetEnd( cell->GetPosition() + cellSize );
            table->AddCell( cell );
        }
    }

    table->ClearEditFlags();
    table->SetFlags(IS_NEW);
    table->Normalize();

    aKID = table->m_Uuid.AsStdString();
    frame->AddToScreen(table, frame->GetScreen());

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}