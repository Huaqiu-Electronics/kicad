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

#include <exception>
#include <quote/quote_controller.h>
#include <utils/httplib_wrapper.h>
#include <quote/pcb_order.h>
#include <nlohmann/json.hpp>
#include <settings/webview_settings_manager.h>
#include <wx/msgdlg.h>
#include <quote/pcb_smt_order.h>
#include <quote/pcb_smt_quote_response.h>
#include <wx/log.h>

using httplib::SSLClient;
using nlohmann::json;


inline auto add_file_form( std::string const& attachment_file_path, httplib::MultipartFormDataItems& items,
                           std::string const& field_name )
{
    // Attach file if path is not empty
    if( !attachment_file_path.empty() )
    {
        std::ifstream file( attachment_file_path, std::ios::binary );

        if( file )
        {
            std::string file_content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
            std::string filename = attachment_file_path.substr( attachment_file_path.find_last_of( "/\\" ) + 1 );
            items.push_back(
                    { field_name, file_content, filename,
                      attachment_file_path.ends_with( ".zip" ) ? "application/zip" : "application/octet-stream" } );
        }
    }
}
std::optional<std::string> QUOTE_CONTROLLER::pcb_smt_quote( PCB_SMT_ORDER const&            form,
                                                            PCB_SMT_QUOTE_FILE_PATHS const& file_paths )
{
    wxLogDebug( "Quote SMT order with form: %s", nlohmann::json( form ).dump() );
    
    httplib::MultipartFormDataItems items;
    httplib::Headers                headers;
    headers.insert(
            std::make_pair( "authorization", WEBVIEW_SETTINGS_MANAGER::get_instance().get_access_token().token ) );

    add_file_form( file_paths.pcb_file_path, items, "pcbFile" );
    add_file_form( file_paths.coord_file_path, items, "coordFile" );
    add_file_form( file_paths.bom_file_path, items, "bomFile" );
    items.push_back( { "reqParseVO", json( form ).dump() } );


    auto res = _client->Post( "/openapi/pcba/saveDfmTmpOrder", headers, items );

    if( res && res->status == 200 )
    {
        try
        {
            return nlohmann::json::parse( res->body ).get<PCB_SMT_QUOTE_RESPONSE>().result.url;
        }

        catch( std::exception& e )
        {
            wxMessageBox( wxString::FromUTF8( e.what() ) );
        }
    }

    return {};
}
