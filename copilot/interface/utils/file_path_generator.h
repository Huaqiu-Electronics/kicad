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

#ifndef FILE_PATH_GENERATOR_H
#define FILE_PATH_GENERATOR_H

#include <copilot_global.h>
#include <optional>
#include <string>
#include <vector>
#include <mutex>

class COPILOT_API FILE_PATH_GENERATOR
{
    FILE_PATH_GENERATOR();

    std::vector<std::string> _tmp_paths;

    std::mutex _mutex;

public:
    ~FILE_PATH_GENERATOR();

    static auto get() -> FILE_PATH_GENERATOR&;

    auto record_for_cleanup( std::string const& aPath ) -> void;

    // Generate a temporary file , with aSuffix if provided e.g. .zip
    auto gen_tmp_file(std::optional<std::string> aSuffix = std::nullopt) -> std::string;

    auto gen_tmp_dir() -> std::string;
};


#endif
