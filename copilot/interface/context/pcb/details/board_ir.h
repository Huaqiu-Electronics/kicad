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

#ifndef BOARD_IR_H
#define BOARD_IR_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace copilot
{

struct VECTOR2
{
    int x;
    int y;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( VECTOR2, x, y )
};

struct TEXT_ITEM
{
    std::string layer;
    std::string text;
    double      x;
    double      y;
    double      rotation;
    std::string hJustify;
    std::string vJustify;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TEXT_ITEM, layer, text, x, y, rotation, hJustify, vJustify )
};

struct LINE_ITEM
{
    std::string layer;
    double      x1;
    double      y1;
    double      x2;
    double      y2;
    double      width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( LINE_ITEM, layer, x1, y1, x2, y2, width )
};

struct CIRCLE_ITEM
{
    std::string layer;
    double      x;
    double      y;
    double      radius;
    double      width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( CIRCLE_ITEM, layer, x, y, radius, width )
};

struct ARC_ITEM
{
    std::string layer;
    VECTOR2     start;
    VECTOR2     end;
    VECTOR2     mid;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( ARC_ITEM, layer, start, end, mid )
};

struct SEGMENT
{
    double x1;
    double y1;
    double x2;
    double y2;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( SEGMENT, x1, y1, x2, y2 )
};

struct ZONE
{
    std::string          layer;
    std::vector<SEGMENT> segments;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( ZONE, layer, segments )
};

struct PAD
{
    std::string net;
    std::string pin;
    std::string type;
    std::string shape;
    double      x;
    double      y;
    double      width;
    double      height;
    double      rotation;
    std::string layer;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PAD, net, pin, type, shape, x, y, width, height, rotation,
                                    layer )
};

struct FOOTPRINT
{
    std::string      ref;
    std::string      value;
    std::string      footprint;
    double           x;
    double           y;
    double           rotation;
    std::vector<PAD> pads;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FOOTPRINT, ref, value, footprint, x, y, rotation, pads )
};

struct REFERENCE_IMAGE_ITEM
{
    std::string layer;
    double      x;
    double      y;
    double      width;
    double      height;
    std::string image_data;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( REFERENCE_IMAGE_ITEM, layer, x, y, width, height, image_data )
};

struct FIELD_ITEM
{
    std::string layer;
    std::string text;
    double      x;
    double      y;
    double      rotation;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FIELD_ITEM, layer, text, x, y, rotation )
};

struct GENERATOR_ITEM
{
    std::string layer;
    std::string generator_id;
    double      x;
    double      y;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( GENERATOR_ITEM, layer, generator_id, x, y )
};

struct TEXTBOX_ITEM
{
    std::string layer;
    std::string text;
    double      x;
    double      y;
    double      width;
    double      height;
    double      rotation;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TEXTBOX_ITEM, layer, text, x, y, width, height, rotation )
};

struct TABLE_ITEM
{
    std::string                           layer;
    double                                x;
    double                                y;
    double                                width;
    double                                height;
    std::vector<std::vector<std::string>> cells;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TABLE_ITEM, layer, x, y, width, height, cells )
};

struct TABLECELL_ITEM
{
    std::string layer;
    std::string text;
    double      x;
    double      y;
    double      width;
    double      height;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TABLECELL_ITEM, layer, text, x, y, width, height )
};

struct MARKER_ITEM
{
    std::string layer;
    double      x;
    double      y;
    std::string message;
    std::string severity;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( MARKER_ITEM, layer, x, y, message, severity )
};

struct DIMENSION_ITEM
{
    std::string layer;
    double      x1;
    double      y1;
    double      x2;
    double      y2;
    std::string value;
    std::string dim_type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( DIMENSION_ITEM, layer, x1, y1, x2, y2, value, dim_type )
};

struct TARGET_ITEM
{
    std::string layer;
    double      x;
    double      y;
    double      diameter;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TARGET_ITEM, layer, x, y, diameter )
};

struct ITEM_LIST
{
    std::vector<std::string> item_ids;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( ITEM_LIST, item_ids )
};

struct NETINFO_ITEM
{
    std::string netname;
    int         netcode;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( NETINFO_ITEM, netname, netcode )
};

struct GROUP_ITEM
{
    std::vector<std::string> item_ids;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( GROUP_ITEM, item_ids )
};

struct BOARD
{
    std::vector<TEXT_ITEM>            texts;
    std::vector<LINE_ITEM>            lines;
    std::vector<CIRCLE_ITEM>          circles;
    std::vector<ARC_ITEM>             arcs;
    std::vector<ZONE>                 zones;
    std::vector<FOOTPRINT>            footprints;
    std::vector<REFERENCE_IMAGE_ITEM> reference_images;
    std::vector<FIELD_ITEM>           fields;
    std::vector<GENERATOR_ITEM>       generators;
    std::vector<TEXTBOX_ITEM>         textboxes;
    std::vector<TABLE_ITEM>           tables;
    std::vector<TABLECELL_ITEM>       tablecells;
    std::vector<MARKER_ITEM>          markers;
    std::vector<DIMENSION_ITEM>       dimensions;
    std::vector<TARGET_ITEM>          targets;
    std::vector<ITEM_LIST>            item_lists;
    std::vector<NETINFO_ITEM>         netinfos;
    std::vector<GROUP_ITEM>           groups;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( BOARD, texts, lines, circles, arcs, zones, footprints,
                                    reference_images, fields, generators, textboxes, tables,
                                    tablecells, markers, dimensions, targets, item_lists, netinfos,
                                    groups )
};

} // namespace copilot

#endif
