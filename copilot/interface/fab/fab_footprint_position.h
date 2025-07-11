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

#ifndef FAB_FOOTPRINT_POSITION_H
#define FAB_FOOTPRINT_POSITION_H

#include <nlohmann/json.hpp>
#include <string>


enum FP_SIDE
{
    TOP,
    BOTTOM
};


struct FP_POSITION
{
    std::string ref;
    std::string value;
    std::string package;
    double      pos_x;
    double      pos_y;
    double      rot;
    FP_SIDE     side;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FP_POSITION, ref, value, package, pos_x, pos_y, rot, side )
};


struct FP_POSITION_LIST
{
    FP_SIDE                  side;
    std::vector<FP_POSITION> positions;
    std::string              unit = "mm";
    std::string              angle = "deg";
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FP_POSITION_LIST, side, positions, unit, angle )
};


#endif
