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

#include <quote/quote_controller.h>
#include <utils/httplib_wrapper.h>
#include <quote/pcb_order.h>

using httplib::SSLClient;
QUOTE_CONTROLLER::QUOTE_CONTROLLER() : _client( std::make_unique<SSLClient>( "www.eda.cn" ) )
{
}

QUOTE_CONTROLLER::~QUOTE_CONTROLLER()
{
}


std::optional<PCB_QUOTE_RESPONSE> QUOTE_CONTROLLER::pcb_quote( PCB_ORDER const& form,
                                                               std::string const&    attachment_file_path )
{
    // Serialize form to JSON
    nlohmann::json j = form;

    // Prepare multipart form data
    httplib::MultipartFormDataItems items = { { "form", j.dump(), "", "application/json" } };

    // Attach file if path is not empty
    if( !attachment_file_path.empty() )
    {
        std::ifstream file( attachment_file_path, std::ios::binary );
        if( file )
        {
            std::string file_content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
            std::string filename = attachment_file_path.substr( attachment_file_path.find_last_of( "/\\" ) + 1 );
            items.push_back( { "file", file_content, filename, "application/octet-stream" } );
        }
    }

    auto res = _client->Post( "/openapi/api/hqpcb/External/fileQuote", items );

    if( res && res->status == 200 )
    {
        nlohmann::json urls = nlohmann::json::parse( res->body );
        for( const auto& key : { "url", "redirect" } )
        {
            if( urls.contains( key ) )
            {
                std::string uat_url = urls[key].get<std::string>();
                // Extract id from the url
                std::string id;
                std::size_t id_pos = uat_url.find( "id=" );
                if( id_pos != std::string::npos )
                {
                    id_pos += 3; // move past "id="
                    std::size_t end_pos = uat_url.find( '&', id_pos );
                    if( end_pos == std::string::npos )
                        id = uat_url.substr( id_pos );
                    else
                        id = uat_url.substr( id_pos, end_pos - id_pos );
                    // id now contains the value after id=
                }
                return PCB_QUOTE_RESPONSE{ id };
            }
        }
    }

    return {};
}
