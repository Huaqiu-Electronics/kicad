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

#ifndef SMT_ORDER_H
#define SMT_ORDER_H

#include <nlohmann/json.hpp>

struct SMT_ORDER
{
    std::string bom_file_name;
    std::string bom_file_path;
    std::string coord_file_name;
    std::string coord_file_path;
    std::string pcb_ban_height;
    std::string pcb_ban_width;
    std::string bom_material_type_number;

    std::string is_plug = "1";
    std::string number = "5";
    std::string patch_pad_number = "1133";
    std::string plug_number = "64";
    std::string single_or_double_technique = "1";
    std::string plug_material_type = "";
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( SMT_ORDER, bom_file_name, bom_file_path, coord_file_name, coord_file_path,
                                    pcb_ban_height, pcb_ban_width, bom_material_type_number, is_plug, number,
                                    patch_pad_number, plug_number, single_or_double_technique, plug_material_type )
};


#endif
