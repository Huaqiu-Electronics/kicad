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

#ifndef API_DESIGN_CONTENT_H
#define API_DESIGN_CONTENT_H

#include <optional>
#include <string>
#include <list>
#include <nlohmann/json.hpp>

struct NAMED_CONTENT
{
    std::string filename;
    std::string content;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( NAMED_CONTENT, filename, content )
};


struct FOOTPRINT_CONENT
{
    NAMED_CONTENT footprint;
    // The 3d models
    std::list<NAMED_CONTENT> models;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(  FOOTPRINT_CONENT, footprint, models )
};


struct COMPONENT_CONTENT
{
    struct Attribute
    {
        std::string name;
        std::string value;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE( Attribute, name, value )
    };

    NAMED_CONTENT               symbol;
    std::list<Attribute>       attributes;
    std::list<FOOTPRINT_CONENT> footprints;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( COMPONENT_CONTENT, symbol, attributes, footprints )
};


struct PCB_CONTENT
{
    NAMED_CONTENT pcb;
    // footprints used in the PCB , in case we are placing only the PCB
    std::list<FOOTPRINT_CONENT> footprints;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCB_CONTENT, pcb, footprints )
};


struct DESIGN_BLOCK_CONTENT
{
    // The root schematic file name
    std::optional<std::string> root;

    // One or more schematic files
    std::list<NAMED_CONTENT> schematics;

    // The PCB file
    std::optional<PCB_CONTENT> pcb;

    // Components used in the schematics
    std::list<COMPONENT_CONTENT> components;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( DESIGN_BLOCK_CONTENT, root, schematics, pcb, components )
};


#endif
