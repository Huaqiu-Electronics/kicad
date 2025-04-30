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

#ifndef PCB_ITEMS_JSONIFY_HELPER_H
#define PCB_ITEMS_JSONIFY_HELPER_H


#include "math/vector2d.h"
#include "pad.h"
#include "pcb_dimension.h"
#include "pcb_generator.h"
#include "pcb_group.h"
#include "pcb_marker.h"
#include "pcb_reference_image.h"
#include "pcb_table.h"
#include "pcb_tablecell.h"
#include "pcb_target.h"
#include "tools/pcb_selection.h"
#include <board_design_settings.h>
#include <string_utils.h>
#include <pcb_edit_frame.h>
#include <context/pcb/pcb_copilot_global_context.h>
#include <context/pcb/details/board_ir.h>
#include <string>
#include <board.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pcb_shape.h>
#include <zone.h>


inline copilot::VECTOR2 toVector2( VECTOR2I b )
{
    return { b.x, b.y };
}

// ...

inline auto jsonify_pcb_items( const PCB_SELECTION& aSelection ) -> copilot::BOARD
{
    copilot::BOARD board;

    for( const auto& selection : aSelection )
    {
        switch( selection->Type() )
        {
        case PCB_FOOTPRINT_T:
        {
            auto               footprint = static_cast<FOOTPRINT*>( selection );
            copilot::FOOTPRINT f;
            f.ref = footprint->GetReference();
            f.value = footprint->GetValue();
            f.footprint = footprint->GetFPID().GetLibItemName();
            f.x = footprint->GetPosition().x;
            f.y = footprint->GetPosition().y;
            f.rotation = footprint->GetOrientationDegrees();

            for( const PAD* pad : footprint->Pads() )
            {
                copilot::PAD p;
                p.pin = pad->GetNumber();
                p.net = pad->GetNetname();
                p.x = pad->GetPosition().x;
                p.y = pad->GetPosition().y;
                p.rotation = pad->GetOrientationDegrees();
                p.type = pad->GetPinType();
                p.shape = pad->ShowPadShape( pad->GetLayer() );
                p.width = pad->GetSizeX();
                p.height = pad->GetSizeY();
                p.layer = LayerName( pad->GetLayer() );
                f.pads.push_back( p );
            }

            board.footprints.push_back( f );
            break;
        }
        case PCB_TEXT_T:
        {
            auto               text = static_cast<PCB_TEXT*>( selection );
            copilot::TEXT_ITEM t;
            t.layer = LayerName( text->GetLayer() );
            t.text = text->GetText();
            t.x = text->GetPosition().x;
            t.y = text->GetPosition().y;
            t.rotation = text->GetDrawRotation().AsDegrees();
            t.hJustify = text->GetHorizJustify();
            t.vJustify = text->GetVertJustify();
            board.texts.push_back( t );
            break;
        }
        case PCB_TRACE_T:
        {
            auto track = dynamic_cast<PCB_TRACK*>( selection );
            if( track )
            {
                copilot::LINE_ITEM l;
                l.layer = LayerName( track->GetLayer() );
                l.x1 = track->GetStart().x;
                l.y1 = track->GetStart().y;
                l.x2 = track->GetEnd().x;
                l.y2 = track->GetEnd().y;
                l.width = track->GetWidth();
                board.lines.push_back( l );
            }
            break;
        }
        case PCB_SHAPE_T:
        {
            auto shape = dynamic_cast<PCB_SHAPE*>( selection );
            if( shape )
            {
                switch( shape->GetShape() )
                {
                case SHAPE_T::SEGMENT:
                {
                    copilot::LINE_ITEM l;
                    l.layer = LayerName( shape->GetLayer() );
                    l.x1 = shape->GetStart().x;
                    l.y1 = shape->GetStart().y;
                    l.x2 = shape->GetEnd().x;
                    l.y2 = shape->GetEnd().y;
                    l.width = shape->GetWidth();
                    board.lines.push_back( l );
                    break;
                }
                case SHAPE_T::CIRCLE:
                {
                    copilot::CIRCLE_ITEM c;
                    c.layer = LayerName( shape->GetLayer() );
                    c.x = shape->GetCenter().x;
                    c.y = shape->GetCenter().y;
                    c.radius = shape->GetRadius();
                    c.width = shape->GetWidth();
                    board.circles.push_back( c );
                    break;
                }
                case SHAPE_T::ARC:
                {
                    copilot::ARC_ITEM a;
                    a.layer = LayerName( shape->GetLayer() );
                    if( auto arc = dynamic_cast<PCB_ARC*>( shape ) )
                    {
                        a.start = toVector2( arc->GetStart() );
                        a.end = toVector2( arc->GetEnd() );
                        a.mid = toVector2( arc->GetMid() );
                    }
                    board.arcs.push_back( a );
                    break;
                }
                default: break;
                }
            }
            break;
        }
        case PCB_ZONE_T:
        {
            auto zone = dynamic_cast<ZONE*>( selection );
            if( zone )
            {
                copilot::ZONE z;
                z.layer = LayerName( zone->GetLayer() );
                const SHAPE_POLY_SET* outline = zone->GetFilledPolysList( zone->GetLayer() ).get();
                if( outline )
                {
                    for( int i = 0; i < outline->OutlineCount(); ++i )
                    {
                        const SHAPE_LINE_CHAIN& chain = outline->Outline( i );
                        for( int j = 0; j < chain.SegmentCount(); ++j )
                        {
                            copilot::SEGMENT s;
                            s.x1 = chain.CPoint( j ).x;
                            s.y1 = chain.CPoint( j ).y;
                            s.x2 = chain.CPoint( ( j + 1 ) % chain.PointCount() ).x;
                            s.y2 = chain.CPoint( ( j + 1 ) % chain.PointCount() ).y;
                            z.segments.push_back( s );
                        }
                    }
                }
                board.zones.push_back( z );
            }
            break;
        }
        case PCB_PAD_T:
        {
            auto         pad = static_cast<PAD*>( selection );
            copilot::PAD p;
            p.pin = pad->GetNumber();
            p.net = pad->GetNetname();
            p.x = pad->GetPosition().x;
            p.y = pad->GetPosition().y;
            p.rotation = pad->GetOrientationDegrees();
            p.type = pad->GetPinType();
            p.shape = pad->ShowPadShape( pad->GetLayer() );
            p.width = pad->GetSizeX();
            p.height = pad->GetSizeY();
            p.layer = LayerName( pad->GetLayer() );
            if( board.footprints.empty() )
            {
                copilot::FOOTPRINT f;
                f.pads.push_back( p );
                board.footprints.push_back( f );
            }
            else
            {
                board.footprints.back().pads.push_back( p );
            }
            break;
        }
        case PCB_VIA_T:
        {
            auto via = dynamic_cast<PCB_VIA*>( selection );
            if( via )
            {
                copilot::PAD p;
                p.pin = "";
                p.net = via->GetNetname();
                p.x = via->GetPosition().x;
                p.y = via->GetPosition().y;
                p.rotation = 0.0;
                p.type = "via";
                p.shape = "circle";
                p.width = via->GetWidth();
                p.height = via->GetWidth();
                p.layer = LayerName( via->GetLayer() );
                if( board.footprints.empty() )
                {
                    copilot::FOOTPRINT f;
                    f.pads.push_back( p );
                    board.footprints.push_back( f );
                }
                else
                {
                    board.footprints.back().pads.push_back( p );
                }
            }
            break;
        }
        case PCB_ARC_T:
        {
            auto arc = dynamic_cast<PCB_ARC*>( selection );
            if( arc )
            {
                copilot::ARC_ITEM a;
                a.layer = LayerName( arc->GetLayer() );
                a.start = toVector2( arc->GetStart() );
                a.end = toVector2( arc->GetEnd() );
                a.mid = toVector2( arc->GetMid() );
                board.arcs.push_back( a );
            }
            break;
        }
        case PCB_REFERENCE_IMAGE_T:
        {
            auto img = dynamic_cast<PCB_REFERENCE_IMAGE*>( selection );
            if( img )
            {
                copilot::REFERENCE_IMAGE_ITEM r;
                r.layer = LayerName( img->GetLayer() );
                r.x = img->GetPosition().x;
                r.y = img->GetPosition().y;
                board.reference_images.push_back( r );
            }
            break;
        }
        case PCB_FIELD_T:
        {
            auto field = dynamic_cast<PCB_FIELD*>( selection );
            if( field )
            {
                copilot::FIELD_ITEM f;
                f.layer = LayerName( field->GetLayer() );
                f.text = field->GetText();
                f.x = field->GetPosition().x;
                f.y = field->GetPosition().y;
                f.rotation = field->GetDrawRotation().AsDegrees();
                board.fields.push_back( f );
            }
            break;
        }
        case PCB_GENERATOR_T:
        {
            auto gen = dynamic_cast<PCB_GENERATOR*>( selection );
            if( gen )
            {
                copilot::GENERATOR_ITEM g;
                g.layer = LayerName( gen->GetLayer() );
                g.generator_id = gen->GetGeneratorType();
                g.x = gen->GetPosition().x;
                g.y = gen->GetPosition().y;
                board.generators.push_back( g );
            }
            break;
        }
        case PCB_TEXTBOX_T:
        {
            auto tb = dynamic_cast<PCB_TEXTBOX*>( selection );
            if( tb )
            {
                copilot::TEXTBOX_ITEM t;
                t.layer = LayerName( tb->GetLayer() );
                t.text = tb->GetText();
                t.x = tb->GetPosition().x;
                t.y = tb->GetPosition().y;
                t.width = tb->GetWidth();
                t.height = tb->GetTextHeight();
                t.rotation = tb->GetDrawRotation().AsDegrees();
                board.textboxes.push_back( t );
            }
            break;
        }
        case PCB_TABLE_T:
        {
            auto table = dynamic_cast<PCB_TABLE*>( selection );
            if( table )
            {
                copilot::TABLE_ITEM t;
                t.layer = LayerName( table->GetLayer() );
                t.x = table->GetPosition().x;
                t.y = table->GetPosition().y;
                t.width = table->GetEnd().x - table->GetPosition().x;
                t.height = table->GetEnd().y - table->GetPosition().y;
                board.tables.push_back( t );
            }
            break;
        }
        case PCB_TABLECELL_T:
        {
            auto cell = dynamic_cast<PCB_TABLECELL*>( selection );
            if( cell )
            {
                copilot::TABLECELL_ITEM c;
                c.layer = LayerName( cell->GetLayer() );
                c.text = cell->GetText();
                c.x = cell->GetPosition().x;
                c.y = cell->GetPosition().y;
                c.width = cell->GetWidth();
                c.height = cell->GetTextHeight();
                board.tablecells.push_back( c );
            }
            break;
        }
        case PCB_MARKER_T:
        {
            auto marker = dynamic_cast<PCB_MARKER*>( selection );
            if( marker )
            {
                copilot::MARKER_ITEM m;
                m.layer = LayerName( marker->GetLayer() );
                m.x = marker->GetPosition().x;
                m.y = marker->GetPosition().y;
                m.message = marker->GetComment();
                m.severity = marker->GetSeverity();
                board.markers.push_back( m );
            }
            break;
        }
        case PCB_DIMENSION_T:
        case PCB_DIM_ALIGNED_T:
        case PCB_DIM_LEADER_T:
        case PCB_DIM_CENTER_T:
        case PCB_DIM_RADIAL_T:
        case PCB_DIM_ORTHOGONAL_T:
        {
            auto dim = dynamic_cast<PCB_DIMENSION_BASE*>( selection );
            if( dim )
            {
                copilot::DIMENSION_ITEM d;
                d.layer = LayerName( dim->GetLayer() );
                d.x1 = dim->GetStart().x;
                d.y1 = dim->GetStart().y;
                d.x2 = dim->GetEnd().x;
                d.y2 = dim->GetEnd().y;
                d.value = dim->GetValueText();
                d.dim_type = dim->GetTypeDesc();
                board.dimensions.push_back( d );
            }
            break;
        }
        case PCB_TARGET_T:
        {
            auto target = dynamic_cast<PCB_TARGET*>( selection );
            if( target )
            {
                copilot::TARGET_ITEM t;
                t.layer = LayerName( target->GetLayer() );
                t.x = target->GetPosition().x;
                t.y = target->GetPosition().y;
                t.diameter = target->GetSize();
                board.targets.push_back( t );
            }
            break;
        }
        case PCB_NETINFO_T:
        {
            auto netinfo = dynamic_cast<NETINFO_ITEM*>( selection );
            if( netinfo )
            {
                copilot::NETINFO_ITEM n;
                n.netname = netinfo->GetNetname();
                n.netcode = netinfo->GetNetCode();
                board.netinfos.push_back( n );
            }
            break;
        }
        case PCB_GROUP_T:
        {
            auto group = dynamic_cast<PCB_GROUP*>( selection );
            if( group )
            {
                copilot::GROUP_ITEM g;
                board.groups.push_back( g );
            }
            break;
        }
        default: break;
        }
    }

    return board;
}


#endif
