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
  "title": "Board",
  "type": "object",
  "properties": {
    "texts": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "text": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "rotation": { "type": "number" },
          "hJustify": { "type": "string" },
          "vJustify": { "type": "string" }
        },
        "required": ["layer", "text", "x", "y", "rotation", "hJustify", "vJustify"]
      }
    },
    "lines": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "x1": { "type": "number" },
          "y1": { "type": "number" },
          "x2": { "type": "number" },
          "y2": { "type": "number" },
          "width": { "type": "number" }
        },
        "required": ["layer", "x1", "y1", "x2", "y2", "width"]
      }
    },
    "circles": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "radius": { "type": "number" },
          "width": { "type": "number" }
        },
        "required": ["layer", "x", "y", "radius", "width"]
      }
    },
    "arcs": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "start": { "$ref": "#/definitions/vector2" },
          "end": { "$ref": "#/definitions/vector2" },
          "mid": { "$ref": "#/definitions/vector2" }
        },
        "required": ["layer", "start", "end", "mid"]
      }
    },
    "zones": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "segments": {
            "type": "array",
            "items": { "$ref": "#/definitions/segment" }
          }
        },
        "required": ["layer", "segments"]
      }
    },
    "footprints": {
      "type": "array",
      "items": {
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
            "items": { "$ref": "#/definitions/pad" }
          }
        },
        "required": ["ref", "value", "footprint", "x", "y", "rotation", "pads"]
      }
    },
    "reference_images": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "width": { "type": "number" },
          "height": { "type": "number" },
          "image_data": { "type": "string" }
        },
        "required": ["layer", "x", "y", "width", "height", "image_data"]
      }
    },
    "fields": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "text": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "rotation": { "type": "number" }
        },
        "required": ["layer", "text", "x", "y", "rotation"]
      }
    },
    "generators": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "generator_id": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" }
        },
        "required": ["layer", "generator_id", "x", "y"]
      }
    },
    "textboxes": {
      "type": "array",
      "items": {
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
        "required": ["layer", "text", "x", "y", "width", "height", "rotation"]
      }
    },
    "tables": {
      "type": "array",
      "items": {
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
        "required": ["layer", "x", "y", "width", "height", "cells"]
      }
    },
    "tablecells": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "text": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "width": { "type": "number" },
          "height": { "type": "number" }
        },
        "required": ["layer", "text", "x", "y", "width", "height"]
      }
    },
    "markers": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "message": { "type": "string" },
          "severity": { "type": "string" }
        },
        "required": ["layer", "x", "y", "message", "severity"]
      }
    },
    "dimensions": {
      "type": "array",
      "items": {
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
        "required": ["layer", "x1", "y1", "x2", "y2", "value", "dim_type"]
      }
    },
    "targets": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layer": { "type": "string" },
          "x": { "type": "number" },
          "y": { "type": "number" },
          "diameter": { "type": "number" }
        },
        "required": ["layer", "x", "y", "diameter"]
      }
    },
    "item_lists": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "item_ids": {
            "type": "array",
            "items": { "type": "string" }
          }
        },
        "required": ["item_ids"]
      }
    },
    "netinfos": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "netname": { "type": "string" },
          "netcode": { "type": "integer" }
        },
        "required": ["netname", "netcode"]
      }
    },
    "groups": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "item_ids": {
            "type": "array",
            "items": { "type": "string" }
          }
        },
        "required": ["item_ids"]
      }
    },
    "vias": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "layers": {
            "type": "array",
            "items": { "type": "string" }
          },
          "drill_value": { "type": "integer" },
          "via_type": { "type": "string" },
          "start": { "$ref": "#/definitions/vector2" },
          "end": { "$ref": "#/definitions/vector2" }
        },
        "required": ["layers", "drill_value", "via_type", "start", "end"]
      }
    },
    "board_length_unit": { "type": "string" }
  },
  "required": ["texts", "lines", "circles", "arcs", "zones", "footprints", "reference_images",
               "fields", "generators", "textboxes", "tables", "tablecells", "markers",
               "dimensions", "targets", "item_lists", "netinfos", "groups", "vias",
               "board_length_unit"],
  "definitions": {
    "vector2": {
      "type": "object",
      "properties": {
        "x": { "type": "integer" },
        "y": { "type": "integer" }
      },
      "required": ["x", "y"]
    },
    "segment": {
      "type": "object",
      "properties": {
        "x1": { "type": "number" },
        "y1": { "type": "number" },
        "x2": { "type": "number" },
        "y2": { "type": "number" }
      },
      "required": ["x1", "y1", "x2", "y2"]
    },
    "pad": {
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
      "required": ["net", "pin", "type", "shape", "x", "y", "width", "height", "rotation", "layer"]
    }
  }
}
)json";
}


#endif
