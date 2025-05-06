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

#ifndef BOARD_JSON_SCHEMA_H
#define BOARD_JSON_SCHEMA_H
namespace copilot
{

constexpr const char* BOARD_JSON_SCHEMA = R"json(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "KiCad BOARD_SELECTIONS Schema",
  "type": "object",
  "description": "Represents the selection of items on a KiCad PCB board.",
  "properties": {
    "texts": {
      "type": "array",
      "description": "List of text items on the PCB."
    },
    "shapes": {
      "type": "object",
      "description": "Geometric shape primitives such as lines, circles, and arcs."
    },
    "arcs": {
      "type": "array",
      "description": "List of arc items on the PCB."
    },
    "tracks": {
      "type": "array",
      "description": "List of track items on the PCB."
    },
    "zones": {
      "type": "array",
      "description": "List of zone items on the PCB."
    },
    "vias": {
      "type": "array",
      "description": "List of via items on the PCB."
    },
    "footprints": {
      "type": "array",
      "description": "List of footprint items on the PCB."
    },
    "reference_images": {
      "type": "array",
      "description": "List of reference image items on the PCB."
    },
    "fields": {
      "type": "array",
      "description": "List of field items on the PCB."
    },
    "generators": {
      "type": "array",
      "description": "List of generator items on the PCB."

    },
    "textboxes": {
      "type": "array",
      "description": "List of textbox items on the PCB."
    },
    "tables": {
      "type": "array",
      "description": "List of table items on the PCB."
    },
    "tablecells": {
      "type": "array",
      "description": "List of table cell items on the PCB."
    },
    "markers": {
      "type": "array",
      "description": "List of marker items on the PCB."
    },
    "dimensions": {
      "type": "array",
      "description": "List of dimension items on the PCB."
    },
    "targets": {
      "type": "array",
      "description": "List of target items on the PCB."
    },
    "item_lists": {
      "type": "array",
      "description": "List of item lists on the PCB."
    },
    "netinfos": {
      "type": "array",
      "description": "List of net information items on the PCB."
    },
    "groups": {
      "type": "array",
      "description": "List of group items on the PCB."
    },
    "board_length_unit": {
      "type": "string",
      "description": "The unit used to describe board lengths, default is 'nm'.",
      "default": "nm"
    }
  }
}
)json";
}


#endif
