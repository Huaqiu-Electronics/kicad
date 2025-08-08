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

#ifndef SEND_DATA_BURIED_POINT_H
#define SEND_DATA_BURIED_POINT_H

#include <http/http_client.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <wx/log.h>
#include <data_buried_point.h>


inline auto send_data_buried_point( std::string const& name )
{
    std::thread t(
            [=]()
            {
                try
                {
                    HTTP_CLIENT http_client;
                    http_client.send_data_buried_point( DATA_BURIED_POINT{ name, fmt::format( "{}_in_kicad", name )

                    } );
                }
                catch( std::exception const& e )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: %s", e.what() ),
                                "send_data_buried_point" );
                }
                catch( ... )
                {
                    wxLogDebug( wxString::Format( "send_data_buried_point error: unknown" ), "send_data_buried_point" );
                }
            } );
    t.detach();
}

#endif
