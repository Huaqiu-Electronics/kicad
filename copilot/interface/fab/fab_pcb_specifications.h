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

// All the size units are in millimeters
struct FAB_PCB_SPECIFICATIONS
{
    int    layer_count;
    double length;
    double width;
    double mini_trace_clearance;
    double mini_trace_width;
    double mini_hole_size;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FAB_PCB_SPECIFICATIONS, layer_count, length, width, mini_trace_clearance,
                                    mini_trace_width, mini_hole_size )
};


#endif
