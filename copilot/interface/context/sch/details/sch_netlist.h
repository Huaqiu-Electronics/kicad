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

#ifndef SCH_NETLIST_H
#define SCH_NETLIST_H


#include <nlohmann/json.hpp>
#include <string>
#include <vector>


namespace copilot
{

struct Field
{
    std::string name;
    std::string value;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( Field, name, value )
};


struct Component
{
    std::string                                  ref;
    std::string                                  value;
    std::string                                  footprint;
    std::unordered_map<std::string, std::string> properties;
    std::vector<Field>                           fields;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( Component, ref, value, footprint, properties, fields )
};

struct Node
{
    std::string ref;
    std::string pin;
    // NOTE The pinfunction is the pin name in fact, which is the pin num by default if the pin name is not defined.
    std::string pinfunction;
    std::string pintype;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( Node, ref, pin, pinfunction, pintype )
};

struct Net
{
    std::string       name;
    std::vector<Node> nodes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( Net, name, nodes )
};

struct Netlist
{
    std::vector<Component> components;
    std::vector<Net>       nets;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( Netlist, components, nets )
};


} // namespace copilot


#endif
