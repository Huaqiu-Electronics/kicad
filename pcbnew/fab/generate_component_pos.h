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

#ifndef GENERATE_COMPONENT_POS_H
#define GENERATE_COMPONENT_POS_H


#include <pcb_edit_frame.h>
#include <exporters/place_file_exporter.h>
#include <utils/file_path_generator.h>
#include <fstream>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>


inline std::vector<FP_POSITION> parse_csv( const std::string& csv )
{
    std::vector<FP_POSITION> result;
    std::istringstream       ss( csv );
    std::string              line;

    // Skip header line
    std::getline( ss, line );

    while( std::getline( ss, line ) )
    {
        if( line.empty() )
            continue;

        FP_POSITION        fp;
        std::istringstream line_ss( line );
        std::string        token;

        auto get_field = [&]( std::string& out )
        {
            if( !std::getline( line_ss, token, ',' ) )
                return false;
            if( !token.empty() && token.front() == '"' && token.back() == '"' )
                token = token.substr( 1, token.size() - 2 );
            out = token;
            return true;
        };

        if( !get_field( fp.Ref ) )
            continue;
        if( !get_field( fp.Val ) )
            continue;
        if( !get_field( fp.Package ) )
            continue;
        if( !get_field( fp.PosX ) )
            continue;
        if( !get_field( fp.PosY ) )
            continue;
        if( !get_field( fp.Rot ) )
            continue;
        if( !get_field( fp.Side ) )
            continue;

        result.push_back( std::move( fp ) );
    }

    return result;
}


template <>
std::vector<FP_POSITION> FAB_INFO_GETTER_PCB::GetComponentPosition()
{
    ReportProgress( _( "Generating component positions" ) );
    PLACE_FILE_EXPORTER exporter( self()->GetBoard(), true, false, false, true, true, true, true, true, false );
    auto                data = exporter.GenPositionData();

    return parse_csv( data );
}


#endif
