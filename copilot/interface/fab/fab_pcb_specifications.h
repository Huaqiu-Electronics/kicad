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

#ifndef FAB_PCB_SPECIFICATIONS_H
#define FAB_PCB_SPECIFICATIONS_H


#include <nlohmann/json.hpp>

//  0.3mm 0.25mm 0.2mm 0.15mm
enum DRILL_HOLE_SIZE
{
    DRILL_HOLE_SIZE_0_3MM,
    DRILL_HOLE_SIZE_0_25MM,
    DRILL_HOLE_SIZE_0_2MM,
    DRILL_HOLE_SIZE_0_15MM,

};

//  10/10mil 8/8mil 6/6mil 5/5mil 4/4mil 3.5/3.5mil
enum TRACE_WITH_OR_SPACING
{
    TRACE_WITH_OR_SPACING_10_10MIL,
    TRACE_WITH_OR_SPACING_8_8MIL,
    TRACE_WITH_OR_SPACING_6_6MIL,
    TRACE_WITH_OR_SPACING_5_5MIL,
    TRACE_WITH_OR_SPACING_4_4MIL,
    TRACE_WITH_OR_SPACING_3_5MIL,
};


struct FAB_PCB_SPECIFICATIONS
{
    int                   layers;                 //The board layer count
    double                length;                 //mm
    double                width;                  //mm
    double                thickness;              //mm
    TRACE_WITH_OR_SPACING mini_trace_space_outer; //The minimal With/Spacing of the trace
    DRILL_HOLE_SIZE       mini_drilled_hole_size; //The minimal hole size

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FAB_PCB_SPECIFICATIONS, layers, length, width, thickness, mini_trace_space_outer,
                                    mini_drilled_hole_size )
};


#endif
