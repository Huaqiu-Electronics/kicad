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

#ifndef PLAIN_API_DESIGN_RESOURCE_H
#define PLAIN_API_DESIGN_RESOURCE_H

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>


struct SYMBOL_RESOURCE
{
    std::string uri;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( SYMBOL_RESOURCE, uri )
};

struct MODEL_RESOURCE
{
    std::string uri;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MODEL_RESOURCE, uri )
};

struct FOOTPRINT_RESOURCE
{
    std::optional<std::string>  uri;
    std::vector<MODEL_RESOURCE> models;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FOOTPRINT_RESOURCE, uri, models )
};

struct COMPONENT_RESOURCE
{
    struct ATTRIBUTE
    {
        std::string name;
        std::string value;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE( ATTRIBUTE, name, value )
    };

    std::optional<std::string>      component_name;
    SYMBOL_RESOURCE                 symbol_resource;
    std::vector<FOOTPRINT_RESOURCE> footprint_resources;
    std::vector<ATTRIBUTE>          attributes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( COMPONENT_RESOURCE, component_name, symbol_resource, footprint_resources,
                                    attributes )
};

struct PCB_RESOURCE
{
    std::string                     pcb_uri;
    std::vector<FOOTPRINT_RESOURCE> footprint_resources;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCB_RESOURCE, pcb_uri, footprint_resources )
};

struct DESIGN_BLOCK_RESOURCE
{
    std::vector<std::string>        schematic_uris;
    std::optional<std::string>      root_schematic;
    std::vector<COMPONENT_RESOURCE> component_resources;
    std::optional<PCB_RESOURCE>     pcb_resource;
    std::optional<std::string>      version;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( DESIGN_BLOCK_RESOURCE, schematic_uris, root_schematic, component_resources,
                                    pcb_resource, version )
};


#endif
