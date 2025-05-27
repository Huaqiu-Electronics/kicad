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

#ifndef API_DESIGN_RESOURCE_H
#define API_DESIGN_RESOURCE_H

#include <optional>
#include <string>
#include <list>
#include <unordered_map>


struct NamedContent
{
    std::string filename;
    std::string content;
};


struct FootprintConent
{
    NamedContent footprint;

    // The 3d models
    std::list<NamedContent> models;
};


struct ComponentContent
{
    NamedContent                                 symbol;
    std::unordered_map<std::string, std::string> attributes;
    std::list<FootprintConent>                   footprints;
};


struct PCBConent
{
    NamedContent pcb;
    // footprints used in the PCB , in case we are placing only the PCB
    std::list<FootprintConent> footprints;
};


struct DesignBlockConent
{
    // The root schematic file name
    std::optional<std::string> root;

    // One or more schematic files
    std::list<NamedContent> schematics;

    // The PCB file
    std::optional<PCBConent> pcb;

    // Components used in the schematics
    std::list<ComponentContent> components;
};

#endif
