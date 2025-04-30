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
  "title": "BOARD",
  "type": "object",
  "description": "Represents a complete board design including various drawing and component items.",
  "properties": {
    "texts": {
      "type": "array",
      "description": "List of text annotations on the board.",
      "items": { "$ref": "#/definitions/TEXT_ITEM" }
    },
    "lines": {
      "type": "array",
      "description": "List of line segments used for board graphics or outlines.",
      "items": { "$ref": "#/definitions/LINE_ITEM" }
    },
    "circles": {
      "type": "array",
      "description": "List of circle elements used in board graphics.",
      "items": { "$ref": "#/definitions/CIRCLE_ITEM" }
    },
    "arcs": {
      "type": "array",
      "description": "List of arcs used in the board's geometry or silkscreen.",
      "items": { "$ref": "#/definitions/ARC_ITEM" }
    },
    "zones": {
      "type": "array",
      "description": "List of copper fill zones defined by segments.",
      "items": { "$ref": "#/definitions/ZONE" }
    },
    "footprints": {
      "type": "array",
      "description": "Component footprints placed on the board.",
      "items": { "$ref": "#/definitions/FOOTPRINT" }
    },
    "reference_images": {
      "type": "array",
      "description": "Embedded reference images used for design aid.",
      "items": { "$ref": "#/definitions/REFERENCE_IMAGE_ITEM" }
    },
    "fields": {
      "type": "array",
      "description": "Fields representing metadata or title block information.",
      "items": { "$ref": "#/definitions/FIELD_ITEM" }
    },
    "generators": {
      "type": "array",
      "description": "Items generated dynamically based on identifiers.",
      "items": { "$ref": "#/definitions/GENERATOR_ITEM" }
    },
    "textboxes": {
      "type": "array",
      "description": "Multi-line text boxes used for annotations.",
      "items": { "$ref": "#/definitions/TEXTBOX_ITEM" }
    },
    "tables": {
      "type": "array",
      "description": "Tabular data rendered on the board.",
      "items": { "$ref": "#/definitions/TABLE_ITEM" }
    },
    "tablecells": {
      "type": "array",
      "description": "Individual cells of rendered tables, possibly positioned separately.",
      "items": { "$ref": "#/definitions/TABLECELL_ITEM" }
    },
    "markers": {
      "type": "array",
      "description": "Markers for warnings, errors, or informational notes.",
      "items": { "$ref": "#/definitions/MARKER_ITEM" }
    },
    "dimensions": {
      "type": "array",
      "description": "Dimensional measurements annotated on the board.",
      "items": { "$ref": "#/definitions/DIMENSION_ITEM" }
    },
    "targets": {
      "type": "array",
      "description": "Target points used for alignment or fiducials.",
      "items": { "$ref": "#/definitions/TARGET_ITEM" }
    },
    "item_lists": {
      "type": "array",
      "description": "Lists of item identifiers for grouping or referencing.",
      "items": { "$ref": "#/definitions/ITEM_LIST" }
    },
    "netinfos": {
      "type": "array",
      "description": "Net information mapping names to numeric codes.",
      "items": { "$ref": "#/definitions/NETINFO_ITEM" }
    },
    "groups": {
      "type": "array",
      "description": "Groups of item identifiers used for logical or visual grouping.",
      "items": { "$ref": "#/definitions/GROUP_ITEM" }
    },
    "length_unit": {
      "type": "string",
      "description": "The unit of length used in the board design."
    }
  }
  "required": [],
  "definitions": {
    "TEXT_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string", "description": "Layer the text belongs to." },
        "text": { "type": "string", "description": "The text content." },
        "x": { "type": "number", "description": "X-coordinate of the text." },
        "y": { "type": "number", "description": "Y-coordinate of the text." },
        "rotation": { "type": "number", "description": "Text rotation in degrees." },
        "hJustify": { "type": "string", "description": "Horizontal justification." },
        "vJustify": { "type": "string", "description": "Vertical justification." }
      },
      "required": ["layer", "text", "x", "y"]
    },
    "LINE_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x1": { "type": "number" },
        "y1": { "type": "number" },
        "x2": { "type": "number" },
        "y2": { "type": "number" },
        "width": { "type": "number" }
      },
      "required": ["layer", "x1", "y1", "x2", "y2"]
    },
    "CIRCLE_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "radius": { "type": "number" },
        "width": { "type": "number" }
      },
      "required": ["layer", "x", "y", "radius"]
    },
    "ARC_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "start": { "$ref": "#/definitions/VECTOR2" },
        "end": { "$ref": "#/definitions/VECTOR2" },
        "mid": { "$ref": "#/definitions/VECTOR2" }
      },
      "required": ["layer", "start", "end", "mid"]
    },
    "VECTOR2": {
      "type": "object",
      "properties": {
        "x": { "type": "integer" },
        "y": { "type": "integer" }
      },
      "required": ["x", "y"]
    },
    "SEGMENT": {
      "type": "object",
      "properties": {
        "x1": { "type": "number" },
        "y1": { "type": "number" },
        "x2": { "type": "number" },
        "y2": { "type": "number" }
      },
      "required": ["x1", "y1", "x2", "y2"]
    },
    "ZONE": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "segments": {
          "type": "array",
          "items": { "$ref": "#/definitions/SEGMENT" }
        }
      },
      "required": ["layer", "segments"]
    },
    "PAD": {
      "type": "object",
      "properties": {
        "net": { "type": "string" },
        "pin": { "type": "string" },
        "type": { "type": "string" },
        "shape": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "width": { "type": "number" },
        "height": { "type": "number" },
        "rotation": { "type": "number" },
        "layer": { "type": "string" }
      },
      "required": ["net", "pin", "x", "y", "width", "height"]
    },
    "FOOTPRINT": {
      "type": "object",
      "properties": {
        "ref": { "type": "string" },
        "value": { "type": "string" },
        "footprint": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "rotation": { "type": "number" },
        "pads": {
          "type": "array",
          "items": { "$ref": "#/definitions/PAD" }
        }
      },
      "required": ["ref", "value", "footprint", "x", "y"]
    },
    "REFERENCE_IMAGE_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "width": { "type": "number" },
        "height": { "type": "number" },
        "image_data": { "type": "string" }
      },
      "required": ["layer", "x", "y", "image_data"]
    },
    "FIELD_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "text": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "rotation": { "type": "number" }
      },
      "required": ["layer", "text", "x", "y"]
    },
    "GENERATOR_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "generator_id": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" }
      },
      "required": ["layer", "generator_id", "x", "y"]
    },
    "TEXTBOX_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "text": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "width": { "type": "number" },
        "height": { "type": "number" },
        "rotation": { "type": "number" }
      },
      "required": ["layer", "text", "x", "y"]
    },
    "TABLE_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "width": { "type": "number" },
        "height": { "type": "number" },
        "cells": {
          "type": "array",
          "items": {
            "type": "array",
            "items": { "type": "string" }
          }
        }
      },
      "required": ["layer", "x", "y", "cells"]
    },
    "TABLECELL_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "text": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "width": { "type": "number" },
        "height": { "type": "number" }
      },
      "required": ["layer", "text", "x", "y"]
    },
    "MARKER_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "message": { "type": "string" },
        "severity": { "type": "string" }
      },
      "required": ["layer", "x", "y", "message"]
    },
    "DIMENSION_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x1": { "type": "number" },
        "y1": { "type": "number" },
        "x2": { "type": "number" },
        "y2": { "type": "number" },
        "value": { "type": "string" },
        "dim_type": { "type": "string" }
      },
      "required": ["layer", "x1", "y1", "x2", "y2"]
    },
    "TARGET_ITEM": {
      "type": "object",
      "properties": {
        "layer": { "type": "string" },
        "x": { "type": "number" },
        "y": { "type": "number" },
        "diameter": { "type": "number" }
      },
      "required": ["layer", "x", "y", "diameter"]
    },
    "ITEM_LIST": {
      "type": "object",
      "properties": {
        "item_ids": {
          "type": "array",
          "items": { "type": "string" }
        }
      },
      "required": ["item_ids"]
    },
    "NETINFO_ITEM": {
      "type": "object",
      "properties": {
        "netname": { "type": "string" },
        "netcode": { "type": "integer" }
      },
      "required": ["netname", "netcode"]
    },
    "GROUP_ITEM": {
      "type": "object",
      "properties": {
        "item_ids": {
          "type": "array",
          "items": { "type": "string" }
        }
      },
      "required": ["item_ids"]
    }
  }
})json";
}


#endif
