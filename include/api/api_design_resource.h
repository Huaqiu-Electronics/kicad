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
#include <nlohmann/json.hpp>


struct NamedContent
{
    std::string filename;
    std::string content;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( NamedContent, filename, content )
};


struct FootprintConent
{
    NamedContent footprint;
    // The 3d models
    std::list<NamedContent> models;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FootprintConent, footprint, models )
};


struct ComponentContent
{
    struct Attribute
    {
        std::string name;
        std::string value;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE( Attribute, name, value )
    };

    NamedContent               symbol;
    std::list<Attribute>       attributes;
    std::list<FootprintConent> footprints;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( ComponentContent, symbol, attributes, footprints )
};


struct PCBConent
{
    NamedContent pcb;
    // footprints used in the PCB , in case we are placing only the PCB
    std::list<FootprintConent> footprints;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCBConent, pcb, footprints )
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
    friend void                 to_json( nlohmann ::json& nlohmann_json_j, const DesignBlockConent& nlohmann_json_t )
    {
        if( nlohmann_json_t.root )
            nlohmann_json_j["root"] = *nlohmann_json_t.root;

        nlohmann_json_j["schematics"] = nlohmann_json_t.schematics;

        if( nlohmann_json_t.pcb )
            nlohmann_json_j["pcb"] = *nlohmann_json_t.pcb;

        nlohmann_json_j["components"] = nlohmann_json_t.components;
    }
    friend void from_json( const nlohmann ::json& nlohmann_json_j, DesignBlockConent& nlohmann_json_t )
    {
        if( nlohmann_json_j.contains( "root" ) )
            nlohmann_json_t.root = nlohmann_json_j.at( "root" ).get<std::string>();

        nlohmann_json_j.at( "schematics" ).get_to( nlohmann_json_t.schematics );

        if( nlohmann_json_j.contains( "pcb" ) )
            nlohmann_json_t.pcb = nlohmann_json_j.at( "pcb" ).get<PCBConent>();

        nlohmann_json_j.at( "components" ).get_to( nlohmann_json_t.components );
    }
};

#endif
