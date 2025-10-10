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

#ifndef RANDOM_SUBSTRING_H
#define RANDOM_SUBSTRING_H

#include <string>
#include <host_type.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <string>


inline std::string random_substring()
{
    static std::mt19937                           gen( std::random_device{}() );
    static std::uniform_real_distribution<double> dist( 0.0, 1.0 );

    double rand_val = dist( gen );

    std::ostringstream oss;
    oss << std::fixed << std::setprecision( 15 ) << rand_val;
    std::string str = oss.str(); // example: "0.123456789012345"

    // extract 5 characters after "0."
    return ( str.size() >= 2 + 5 ) ? str.substr( 2, 5 ) : "00000";
}

#endif
