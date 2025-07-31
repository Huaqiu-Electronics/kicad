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

#ifndef FAB_INFO_GETTER_PCB_H
#define FAB_INFO_GETTER_PCB_H

#include <array>
#include <dialog_gen_footprint_position.h>
#include <dialog_plot.h>
#include <core/typeinfo.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pcb_edit_frame.h>
#include <board.h>
#include <board_design_settings.h>
#include <base_units.h>
#include <tools/pcb_actions.h>
#include <webview/dialog_webview.h>
#include <wx/string.h>
#include <plotcontroller.h>
#include <exporters/gendrill_Excellon_writer.h>


template <>
FAB_PCB_SPECIFICATIONS FAB_INFO_GETTER_PCB::GetPCBSpecifications()
{
    const auto board = self()->GetBoard();
    const auto layers = board->GetCopperLayerCount();
    const auto height = board->GetBoundingBox().GetHeight();
    const auto width = board->GetBoundingBox().GetWidth();

    auto       minTraceWidth = board->GetDesignSettings().m_TrackMinWidth;
    const auto minTraceClearance = board->GetDesignSettings().m_MinClearance;
    auto       minHoleSize =
            std::min( board->GetDesignSettings().m_MinThroughDrill, board->GetDesignSettings().m_ViasMinSize );


    for( const auto& track : board->Tracks() )
    {
        switch( track->Type() )
        {
        case PCB_VIA_T: minHoleSize = std::min( minHoleSize, static_cast<PCB_VIA*>( track )->GetDrillValue() ); break;
        case PCB_TRACE_T:
        case PCB_ARC_T: minTraceWidth = std::min( minTraceWidth, track->GetWidth() ); break;
        default: break;
        }
    }

    return { layers,
             static_cast<double>( height ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( width ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minTraceClearance ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minTraceWidth ) / pcbIUScale.IU_PER_MM,
             static_cast<double>( minHoleSize ) / pcbIUScale.IU_PER_MM };
}


template <>
bool FAB_INFO_GETTER_PCB::GenerateGerberFiles( wxString const& aOutDir )
{
    auto            editor = static_cast<PCB_EDIT_FRAME*>( this );
    auto            board = editor->GetBoard();
    PLOT_CONTROLLER plotter( board );
    auto&           popt = plotter.GetPlotOptions();

    popt.SetOutputDirectory( aOutDir );
    // Set Gerber format
    popt.SetFormat( PLOT_FORMAT::GERBER );

    // General options
    popt.SetPlotValue( true );
    popt.SetPlotReference( true );
    popt.SetPlotFPText( false );
    popt.SetSketchPadsOnFabLayers( false );

    // Gerber options
    popt.SetUseGerberProtelExtensions( false );
    popt.SetCreateGerberJobFile( false );
    popt.SetSubtractMaskFromSilk( true );
    popt.SetUseAuxOrigin( true );
    popt.SetUseGerberX2format( true );
    popt.SetIncludeGerberNetlistInfo( true );
    popt.SetDisableGerberMacros( false );
    popt.SetDrillMarksType( DRILL_MARKS::NO_DRILL_SHAPE );
    popt.SetPlotFrameRef( false );

    // Remove existing files in output directory (optional, not implemented here)

    int layer_count = board->GetCopperLayerCount();

    struct LayerInfo
    {
        std::string  name;
        PCB_LAYER_ID id;
        std::string  description;
    };

    static const std::vector<LayerInfo> k_plot_plan_top = { { "CuTop", F_Cu, "Top layer" },
                                                            { "SilkTop", F_SilkS, "Silk top" },
                                                            { "MaskTop", F_Mask, "Mask top" },
                                                            { "PasteTop", F_Paste, "Paste top" } };
    static const std::vector<LayerInfo> k_plot_plan_bottom = {
        { "CuBottom", B_Cu, "Bottom layer" },    { "SilkBottom", B_SilkS, "Silk bottom" },
        { "MaskBottom", B_Mask, "Mask bottom" }, { "PasteBottom", B_Paste, "Paste bottom" },
        { "EdgeCuts", Edge_Cuts, "Edges" },      { "VScore", Cmts_User, "V score cut" }
    };

    std::vector<LayerInfo> plot_plan;

    if( layer_count == 1 )
    {
        plot_plan = k_plot_plan_top;
        plot_plan.push_back( k_plot_plan_bottom[4] ); // EdgeCuts
        plot_plan.push_back( k_plot_plan_bottom[5] ); // VScore
    }
    else if( layer_count == 2 )
    {
        plot_plan = k_plot_plan_top;
        plot_plan.insert( plot_plan.end(), k_plot_plan_bottom.begin(), k_plot_plan_bottom.end() );
    }
    else
    {
        static const std::vector<LayerInfo> kInnerCopperLayersInfo = {
            { "CuIn1", PCB_LAYER_ID::In1_Cu, "Inner layer 1" },
            { "CuIn2", PCB_LAYER_ID::In2_Cu, "Inner layer 2" },
            { "CuIn3", PCB_LAYER_ID::In3_Cu, "Inner layer 3" },
            { "CuIn4", PCB_LAYER_ID::In4_Cu, "Inner layer 4" },
            { "CuIn5", PCB_LAYER_ID::In5_Cu, "Inner layer 5" },
            { "CuIn6", PCB_LAYER_ID::In6_Cu, "Inner layer 6" },
            { "CuIn7", PCB_LAYER_ID::In7_Cu, "Inner layer 7" },
            { "CuIn8", PCB_LAYER_ID::In8_Cu, "Inner layer 8" },
            { "CuIn9", PCB_LAYER_ID::In9_Cu, "Inner layer 9" },
            { "CuIn10", PCB_LAYER_ID::In10_Cu, "Inner layer 10" },
            { "CuIn11", PCB_LAYER_ID::In11_Cu, "Inner layer 11" },
            { "CuIn12", PCB_LAYER_ID::In12_Cu, "Inner layer 12" },
            { "CuIn13", PCB_LAYER_ID::In13_Cu, "Inner layer 13" },
            { "CuIn14", PCB_LAYER_ID::In14_Cu, "Inner layer 14" },
            { "CuIn15", PCB_LAYER_ID::In15_Cu, "Inner layer 15" },
            { "CuIn16", PCB_LAYER_ID::In16_Cu, "Inner layer 16" },
            { "CuIn17", PCB_LAYER_ID::In17_Cu, "Inner layer 17" },
            { "CuIn18", PCB_LAYER_ID::In18_Cu, "Inner layer 18" },
            { "CuIn19", PCB_LAYER_ID::In19_Cu, "Inner layer 19" },
            { "CuIn20", PCB_LAYER_ID::In20_Cu, "Inner layer 20" },
            { "CuIn21", PCB_LAYER_ID::In21_Cu, "Inner layer 21" },
            { "CuIn22", PCB_LAYER_ID::In22_Cu, "Inner layer 22" },
            { "CuIn23", PCB_LAYER_ID::In23_Cu, "Inner layer 23" },
            { "CuIn24", PCB_LAYER_ID::In24_Cu, "Inner layer 24" },
            { "CuIn25", PCB_LAYER_ID::In25_Cu, "Inner layer 25" },
            { "CuIn26", PCB_LAYER_ID::In26_Cu, "Inner layer 26" },
            { "CuIn27", PCB_LAYER_ID::In27_Cu, "Inner layer 27" },
            { "CuIn28", PCB_LAYER_ID::In28_Cu, "Inner layer 28" },
            { "CuIn29", PCB_LAYER_ID::In29_Cu, "Inner layer 29" },
            { "CuIn30", PCB_LAYER_ID::In30_Cu, "Inner layer 30" }
        };

        plot_plan = k_plot_plan_top;
        const int inner_layers_count = std::min( layer_count - 2, static_cast<int>( kInnerCopperLayersInfo.size() ) );

        for( int i = 0; i < inner_layers_count; ++i )
            plot_plan.push_back( kInnerCopperLayersInfo.at( i ) );


        plot_plan.insert( plot_plan.end(), k_plot_plan_bottom.begin(), k_plot_plan_bottom.end() );
    }

    std::vector<std::string> gerber_files;

    for( const auto& layer_info : plot_plan )
    {
        if( layer_info.id <= B_Cu )
            popt.SetSkipPlotNPTH_Pads( true );
        else
            popt.SetSkipPlotNPTH_Pads( false );

        plotter.SetLayer( layer_info.id );
        plotter.OpenPlotfile( layer_info.name, PLOT_FORMAT::GERBER );
        if( plotter.PlotLayer() )
        {
            wxString file = plotter.GetPlotFileName();
            gerber_files.push_back( file.ToStdString() );
        }
    }
    plotter.ClosePlot();

    return true;
}


template <>
bool FAB_INFO_GETTER_PCB::GenerateDrillFile( wxString const& aOutDir )
{
    auto editor = static_cast<PCB_EDIT_FRAME*>( this );
    auto board = editor->GetBoard();

    EXCELLON_WRITER drlwriter( board );
    bool            mirror = false;
    bool            minimalHeader = false;
    const VECTOR2I  offset = board->GetDesignSettings().GetAuxOrigin();
    bool            mergeNPTH = false;

    drlwriter.SetOptions( mirror, minimalHeader, offset, mergeNPTH );
    drlwriter.SetFormat( false ); // Use default format (not metric)
    bool genDrl = true;
    bool genMap = true;

    drlwriter.CreateDrillandMapFilesSet( aOutDir, genDrl, genMap );
    return true;
}

template <>
std::vector<FP_POSITION_LIST> FAB_INFO_GETTER_PCB::GetComponentPosition()
{
    DIALOG_GEN_FOOTPRINT_POSITION dlg( self() );
    dlg.ShowModal();

    return {};
}

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
    for( size_t ii = list.size(); ii > 0; ii-- )
    {
        BOM_ENTRY* curEntry = *list.begin();
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
