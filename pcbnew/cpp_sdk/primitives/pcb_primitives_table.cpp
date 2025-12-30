#include <pcb_text.h>
#include <pcb_textbox.h>
#include <pcb_table.h>
#include <widgets/appearance_controls.h>
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include <board_design_settings.h>
#include <board_commit.h>
#include "pcb_primitives_table.h"

PCB_PRIMITIVES_TABLE::PCB_PRIMITIVES_TABLE( PCB_BASE_EDIT_FRAME* aFrame ) : PCB_PRIMITIVES_ITEM( aFrame )
{

}


KI_SDK_EXEC_RESULT PCB_PRIMITIVES_TABLE::CreateTable( const VECTOR2I& position, const VECTOR2I& gridSize,
                                                              int rows, int cols, std::string& aKID )
{

    const BOARD_DESIGN_SETTINGS&  bds = GetBaseEditFrame()->GetDesignSettings();
    PCB_LAYER_ID                  layer = GetBaseEditFrame()->GetActiveLayer();
    if( !GetBaseEditFrame()->GetCanvas()->GetView()->IsLayerVisible( layer ) )
    {
        GetBaseEditFrame()->GetAppearancePanel()->SetLayerVisible( layer, true );
        GetBaseEditFrame()->GetCanvas()->Refresh();
    }

    PCB_TABLE* table = new PCB_TABLE(GetBoard(), bds.GetLineThickness(layer));


    table->SetLayer(layer);
    table->SetFlags(IS_NEW);
    table->SetColCount(1);
    table->AddCell( new PCB_TABLECELL( table ));

    table->SetPosition(position);


    table->ClearCells();
    table->SetColCount( cols );



    // BOARD_COMMIT commit( GetBaseEditFrame() );

    VECTOR2I cellSize{gridSize.x * 20, gridSize.y * 2};

    for( int col = 0; col < cols; ++col )
        table->SetColWidth( col, cellSize.x );

    for( int row = 0; row < rows; ++row )
    {
            table->SetRowHeight( row, cellSize.y );

            for( int col = 0; col < cols; ++col )
            {
                PCB_TABLECELL* cell = new PCB_TABLECELL( table );
                cell->SetPosition( position + VECTOR2I( col * cellSize.x, row * cellSize.y ) );
                cell->SetEnd( cell->GetPosition() + cellSize );
                table->AddCell( cell );
            }
    }

    // GetBaseEditFrame()->SetMsgPanel( table );
    table->Normalize();
    // commit.Add( table, GetBaseEditFrame()->GetScreen() );
    // commit.Push(_("Draw Table"));

    GetBoard()->Add( table );

    GetBaseEditFrame()->GetCanvas()->GetView()->Add( table );
    GetBaseEditFrame()->GetCanvas()->Refresh();


    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}