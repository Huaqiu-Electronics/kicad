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

#ifndef PLACE_ORDER_FOR_SMT_H
#define PLACE_ORDER_FOR_SMT_H

#include "place_order_utils.h"

#include <quote/pcb_smt_form.h>
#include <vector>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <utils/file_path_generator.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

inline auto write_positions_to_csv( std::vector<FP_POSITION> const& aPositions, std::string const& aPath )
{
    std::ofstream ofs( aPath, std::ios::binary );
    if( !ofs.is_open() )
        return;

    // Write UTF-8 BOM
    const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    ofs.write( reinterpret_cast<const char*>( bom ), sizeof( bom ) );

    // Write header
    ofs << "Designator,Footprint,PosX,PosY,Layer,Rotation,Comment\n";

    for( auto const& pos : aPositions )
    {
        ofs << pos.Ref << ',' << pos.Package << ',' << pos.PosX << "mm," << pos.PosY << "mm,"
            << ( pos.Side == "top" ? "T" : "B" ) << ',' << pos.Rot << ',' << pos.Val << '\n';
    }
}


inline std::string csv_escape( const std::string& field )
{
    bool needs_quotes = field.find_first_of( ",\"\n\r" ) != std::string::npos;
    if( !needs_quotes )
        return field;

    std::string escaped;
    escaped.reserve( field.size() + 2 );
    escaped.push_back( '"' );
    for( char c : field )
    {
        if( c == '"' )
            escaped += "\"\""; // escape quotes
        else
            escaped.push_back( c );
    }
    escaped.push_back( '"' );
    return escaped;
}

inline auto write_bom_to_csv( const std::string& aFilename, const std::vector<FAB_BOM_ITEM>& aBom )
{
    std::ofstream ofs( aFilename, std::ios::binary ); // binary to control BOM writing
    if( !ofs.is_open() )
        return;

    // Write UTF-8 BOM
    const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    ofs.write( reinterpret_cast<const char*>( bom ), sizeof( bom ) );

    // Header
    ofs << "序号,型号,规格参数,封装,品牌,位号,单机用量\n";

    // Rows
    for( size_t i = 0; i < aBom.size(); ++i )
    {
        const auto& item = aBom[i];

        // Join designators
        std::ostringstream desig_ss;
        for( size_t j = 0; j < item.designators.size(); ++j )
        {
            if( j > 0 )
                desig_ss << ",";
            desig_ss << item.designators[j];
        }

        ofs << ( i + 1 ) << "," << csv_escape( item.mpn ) << "," << csv_escape( item.name ) << ","
            << csv_escape( item.footprint ) << ","
            << "" << "," // brand empty
            << csv_escape( desig_ss.str() ) << "," << item.quantity << "\n";
    }
}


template <>
std::optional<std::string> FAB_INFO_GETTER_PCB::PlaceOrderForSMT()
{
    auto zipFilePath = GetPCBFileZip();

    if( !zipFilePath )
    {
        return {};
    }

    const auto pcb_form = GetPCB_FORM();
    const auto smt_form = GetSTM_FORM();
    smt_form->pcb_ban_height = pcb_form->bheight;
    smt_form->pcb_ban_width = pcb_form->blength;

    const auto bom = GetBOM();
    smt_form->bom_material_type_number = std::to_string( bom.size() );

    const auto positions = GetComponentPosition();
    const auto tmp_dir = FILE_PATH_GENERATOR::get().gen_tmp_dir();
    const auto tmp_fp_prefix =
            ( std::filesystem::path( tmp_dir ) / ( get_board_filename( self()->GetBoard() ) ) ).string();

    const auto position_fp = tmp_fp_prefix + "-positions.csv";
    write_positions_to_csv( positions, position_fp );
    const auto bom_fp = tmp_fp_prefix + "-bom.csv";
    write_bom_to_csv( bom_fp, bom );

    PCB_SMT_QUOTE_FILE_PATHS paths;
    paths.pcb_file_path = zipFilePath->ToStdString();
    paths.coord_file_path = position_fp;
    paths.bom_file_path = bom_fp;


    ReportProgress( _( "Sending Order" ) );
    QUOTE_CONTROLLER controller;
    auto             rsp = controller.smt_quote( PCB_SMT_FORM{ *pcb_form, *smt_form }, paths );

    if( !rsp.has_value() || rsp->empty() )
    {
        ReportProgress( _( "Failed to send order" ) );
        return {};
    }

    return fmt::format( "{}&token={}", *rsp, WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token );
}


#endif
