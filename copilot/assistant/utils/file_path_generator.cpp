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

#include <iostream>
#include <mutex>
#include <utils/file_path_generator.h>
#include <utils/generate_uuid.h>
#include <filesystem>
#include <wx/filename.h>

FILE_PATH_GENERATOR::FILE_PATH_GENERATOR()
{
}

FILE_PATH_GENERATOR::~FILE_PATH_GENERATOR()
{
    try
    {
        for( auto& path : _tmp_paths )
            std::filesystem::remove_all( path );
    }
    catch( std::filesystem::filesystem_error& e )
    {
        std::cerr << e.what() << '\n';
    }
    catch( ... )
    {
        std::cerr << "Unknown error" << '\n';
    }

    _tmp_paths.clear();
}


auto FILE_PATH_GENERATOR::get() -> FILE_PATH_GENERATOR&
{
    static FILE_PATH_GENERATOR instance;
    return instance;
}

auto FILE_PATH_GENERATOR::gen_tmp_file( std::optional<std::string> aSuffix ) -> std::string
{
    auto tmp_fn = ( wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + generate_uuid() ).ToStdString();

    if( aSuffix.has_value() )
    {
        tmp_fn += aSuffix.value();
    }

    record_for_cleanup( tmp_fn );
    return tmp_fn;
}

auto FILE_PATH_GENERATOR::gen_tmp_dir() -> std::string
{
    auto tmpDir = ( wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + generate_uuid() ).ToStdString();
    std::filesystem::create_directories( tmpDir );
    record_for_cleanup( tmpDir );
    return tmpDir;
}


auto FILE_PATH_GENERATOR::record_for_cleanup( std::string const& aPath ) -> void
{
    std::lock_guard<decltype( _mutex )> guard( _mutex );
    _tmp_paths.push_back( aPath );
}