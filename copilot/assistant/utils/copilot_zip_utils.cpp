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

#include <utils/copilot_zip_utils.h>

#include <filesystem>
#include <zip.h>
#include <ioapi.h>
#include <fstream>
#include <wx/msgdlg.h>
COPILOT_ZIP_UTILS::COPILOT_ZIP_UTILS()
{
}

COPILOT_ZIP_UTILS::~COPILOT_ZIP_UTILS()
{
}


bool COPILOT_ZIP_UTILS::zip( std::string const& output_zip, std::string const& input_directory )
{
    zipFile zf = zipOpen( output_zip.c_str(), APPEND_STATUS_CREATE );
    if( zf )
    {
        for( const auto& entry : std::filesystem::directory_iterator( input_directory ) )
        {
            if( entry.is_regular_file() )
            {
                std::ifstream     file( entry.path(), std::ios::binary );
                std::vector<char> buffer( ( std::istreambuf_iterator<char>( file ) ),
                                          std::istreambuf_iterator<char>() );
                zip_fileinfo      zi = {};
                zipOpenNewFileInZip( zf, entry.path().filename().string().c_str(), &zi, nullptr, 0, nullptr, 0, nullptr,
                                     Z_DEFLATED, Z_DEFAULT_COMPRESSION );
                zipWriteInFileInZip( zf, buffer.data(), static_cast<unsigned int>( buffer.size() ) );
                zipCloseFileInZip( zf );
            }
        }
        zipClose( zf, nullptr );
    }
    else
    {
        wxMessageBox( "Error opening zip file", "Error", wxOK | wxICON_ERROR );
        return false;
    }

    return true;
}
