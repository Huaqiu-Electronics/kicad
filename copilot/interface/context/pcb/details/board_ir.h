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

struct TEXT_ITEM
{
    std::string type;
    std::string layer;
    std::string text;
    double      x;
    double      y;
    double      rotation;
    std::string hJustify;
    std::string vJustify;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TEXT_ITEM, type, layer, text, x, y, rotation, hJustify,
                                    vJustify )
};

struct LINE_ITEM
{
    std::string type;
    std::string layer;
    double      x1;
    double      y1;
    double      x2;
    double      y2;
    double      width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( LINE_ITEM, type, layer, x1, y1, x2, y2, width )
};

struct CIRCLE_ITEM
{
    std::string type;
    std::string layer;
    double      x;
    double      y;
    double      radius;
    double      width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( CIRCLE_ITEM, type, layer, x, y, radius, width )
};

struct ARC_ITEM
{
    std::string type;
    std::string layer;
    double      x;
    double      y;
    double      radius;
    double      start;
    double      end;
    double      width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( ARC_ITEM, type, layer, x, y, radius, start, end, width )
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

struct BOARD
{
    std::vector<TEXT_ITEM>   texts;
    std::vector<LINE_ITEM>   lines;
    std::vector<CIRCLE_ITEM> circles;
    std::vector<ARC_ITEM>    arcs;
    std::vector<ZONE>        zones;
    std::vector<FOOTPRINT>   footprints;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( BOARD, texts, lines, circles, arcs, zones, footprints )
};

} // namespace copilot

#endif
