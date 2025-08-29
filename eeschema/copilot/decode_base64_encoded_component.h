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

#ifndef DECODE_BASE64_ENCODED_COMPONENT_H
#define DECODE_BASE64_ENCODED_COMPONENT_H

#include "api/api_design_content.h"
#include <utils/base64.hpp>
inline auto decode_base64_encoded_component( COMPONENT_CONTENT& component )
{
    component.symbol.content = base64::from_base64( component.symbol.content );

    for( auto& fp : component.footprints )
    {
        fp.footprint.content = base64::from_base64( fp.footprint.content );

        for( auto& model : fp.models )
        {
            model.content = base64::from_base64( model.content );
        }
    }
}

#endif
