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

#ifndef PCB_ORDER_H
#define PCB_ORDER_H

#include <nlohmann/json.hpp>

struct PCB_ORDER
{
    std::string address;
    std::string bankong;
    std::string bcount;
    std::string bga;
    std::string bheight;
    std::string blayer;
    std::string blength;
    std::string blind;
    std::string bwidth;
    std::string charcolor;
    std::string cjarea;
    std::string cjh;
    std::string color;
    std::string copper;
    std::string cover;
    std::string express;
    std::string impendance;
    std::string insidecopper;
    std::string invoice_type;
    std::string layoutx;
    std::string layouty;
    std::string lineweight;
    std::string pbnum;
    std::string pcb_file_name;
    std::string pcb_file_path;
    std::string spray;
    std::string testpoint;
    std::string tid;
    std::string union_id;
    std::string union_type;
    std::string units;
    std::string via_in_pad;
    std::string vias;
    std::string zknum;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCB_ORDER, address, bankong, bcount, bga, bheight, blayer, blength, blind, bwidth,
                                    charcolor, cjarea, cjh, color, copper, cover, express, impendance, insidecopper,
                                    invoice_type, layoutx, layouty, lineweight, pbnum, pcb_file_name, pcb_file_path,
                                    spray, testpoint, tid, union_id, union_type, units, via_in_pad, vias, zknum )
};


#endif
