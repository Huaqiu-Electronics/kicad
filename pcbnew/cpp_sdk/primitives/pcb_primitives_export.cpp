#include "pcb_primitives_export.h"
#include <cpp_sdk/utils/ki_sdk_pcb_utils.h>
#include <exporters/export_gencad_writer.h>
#include <board_design_settings.h>

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportOdbPlusPlus( const std::string& aFileName )
{
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportDSNFile( const std::string& aFileName )
{
    PcbUtilsContext    context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        wxLogError( "Failed to get pcb frame pointer." );
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD*          board = context.board;
    PCB_EDIT_FRAME* frame = context.frame;


    bool res = frame->ExportSpecctraFile( aFileName );

    return res ? KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS : KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportGenCAD( const std::string& aFileName , bool aUseAuxOrigin, 
                                                        bool aFilpBottomPads, bool aUniquePinName,
                                                        bool aIndividualShapes, bool aStoreOriginCoords)
{
    PcbUtilsContext    context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        wxLogError( "Failed to get pcb frame pointer." );
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD*          board = context.board;
    PCB_EDIT_FRAME* frame = context.frame;

    GENCAD_EXPORTER exporter( board );


    VECTOR2I GencadOffset;
    VECTOR2I auxOrigin = board->GetDesignSettings().GetAuxOrigin();
    GencadOffset.x = aUseAuxOrigin ? auxOrigin.x : 0;
    GencadOffset.y = aUseAuxOrigin ? auxOrigin.y : 0;

    exporter.SetPlotOffet( GencadOffset );
    exporter.FlipBottomPads( aFilpBottomPads );
    exporter.UsePinNamesUnique( aUniquePinName );
    exporter.UseIndividualShapes( aIndividualShapes );
    exporter.StoreOriginCoordsInFile( aStoreOriginCoords );

    bool res = exporter.WriteFile( aFileName );
    return res ? KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS : KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportVRML( const std::string& aFileName )
{
    PcbUtilsContext    context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        wxLogError( "Failed to get pcb frame pointer." );
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD*          board = context.board;
    PCB_EDIT_FRAME* frame = context.frame;

    bool res = frame->ExportVRML_File( aFileName, 1.0000, true, true, false, false, "", 0., 0. );

    return res ? KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS : KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportIDFV3( const std::string& aFileName )
{
    PcbUtilsContext    context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        wxLogError( "Failed to get pcb frame pointer." );
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    BOARD*          board = context.board;
    PCB_EDIT_FRAME* frame = context.frame;


    bool res = frame->Export_IDF3( board, aFileName, false, 0., 0., true, true );
    return res ? KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS : KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
}

KI_SDK_EXEC_RESULT PCB_PRIMITIVES_EXPORT::ExportGerber( const std::string& aFileName )
{
    PcbUtilsContext    context{};
    KI_SDK_EXEC_RESULT status = GetPcbUtilsContext( &context );

    if( KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS != status || !context.IsValid() )
    {
        wxLogError( "Failed to get pcb frame pointer." );
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }
    

    BOARD*          board = context.board;
    PCB_EDIT_FRAME* frame = context.frame;

    

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}
