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

#ifndef GET_PCB_THICKNESS_H
#define GET_PCB_THICKNESS_H

#include <vector>
#include <string>
#include <map>
inline std::string get_pcb_thickness( int aLayer, double aBrdThickness )
{
    static const std::map<int, std::vector<std::pair<std::string, double>>> THICKNESS_SETTING = {
        { 1, { { "0.6", 0.6 }, { "0.8", 0.8 }, { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 } } },
        { 2, { { "0.6", 0.6 }, { "0.8", 0.8 }, { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 } } },
        { 4, { { "0.6", 0.6 }, { "0.8", 0.8 }, { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 } } },
        { 6, { { "0.8", 0.8 }, { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 }, { "2.5", 2.5 } } },
        { 8, { { "1.0", 1.0 }, { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 }, { "2.5", 2.5 } } },
        { 10, { { "1.2", 1.2 }, { "1.6", 1.6 }, { "2.0", 2.0 }, { "2.5", 2.5 } } },
        { 12, { { "1.6", 1.6 }, { "2.0", 2.0 }, { "2.5", 2.5 } } },
        { 14, { { "1.6", 1.6 }, { "2.0", 2.0 }, { "2.5", 2.5 }, { "3.0", 3.0 } } },
        { 16, { { "2.0", 2.0 }, { "2.5", 2.5 }, { "3.0", 3.0 } } },
        { 18, { { "2.0", 2.0 }, { "2.5", 2.5 }, { "3.0", 3.0 }, { "3.2", 3.2 } } },
        { 20, { { "2.0", 2.0 }, { "2.5", 2.5 }, { "3.0", 3.0 }, { "3.2", 3.2 } } }
    };

    if( auto setting = THICKNESS_SETTING.find( aLayer ); setting != THICKNESS_SETTING.end() )
    {
        const auto& available_thicknesses = setting->second;
        
        for( const auto& thickness : available_thicknesses )
        {
            if( thickness.second >= aBrdThickness )
            {
                return thickness.first;
            }
        }

        return available_thicknesses.back().first;
    }

    return "3.2";
}

#endif
