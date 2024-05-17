/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016-2023 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KICAD_HTTP_HQ_CONNECTION_H
#define KICAD_HTTP_HQ_CONNECTION_H

#include <any>
#include <boost/algorithm/string.hpp>

#include "http_lib/http_lib_settings.h"
#include <kicad_curl/kicad_curl_easy.h>
#include <http_lib/http_lib_connection.h>


// extern const char* const traceHTTPLib;


class HTTP_HQ_CONNECTION : public HTTP_LIB_CONNECTION
{
public:
    static const long DEFAULT_TIMEOUT = 10;

    HTTP_HQ_CONNECTION( const HTTP_HQ_LIB_SOURCE& aSource );

    ~HTTP_HQ_CONNECTION();


    bool QueryParts( const std::vector<std::pair<std::string, std::string>>& aFields );
    std::string GetJsonPostFields( const std::vector<std::pair<std::string, std::string>>& aFields );

    // bool SelectOne( const std::string& aPartID, HTTP_LIB_PART& aFetchedPart );

    /**
     * Retrieves all parts from a specific category from the HTTP library.
     * @param aPk is the primary key of the category
     * @param aResults will be filled with all parts in that category
     * @return true if the query succeeded and at least one part was found, false otherwise
     */
    // bool SelectAll( const HTTP_LIB_CATEGORY& aCategory, std::vector<HTTP_LIB_PART>& aParts );

    std::string GetLastError() const { return m_lastError; }

    std::vector<HTTP_HQ_CATEGORY> getCategories() const { return m_categories; }

    std::vector<HTTP_HQ_PART> getParts() const { return m_parts; }
    
    // HTTP_HQ_PART& getPart( const std::string& aMpn )
    // {
    //     return m_cachedParts.at( aMpn );
    // }

    // auto getCachedParts() { return m_cache; }

    std::string GetFieldsFromSource();

    void SetHttpSource( const HTTP_HQ_LIB_SOURCE& aSource );

    bool RequestCategories() { return syncCategories(); }

    bool RequestPartDetails( HTTP_HQ_PART& aPart );
    bool DownloadLibs( std::string aType, HTTP_HQ_PART& aPart );
    wxString GetLibSavePath( std::string aType, HTTP_HQ_PART& aPart );

    std::string SafeGetString( const nlohmann::json& obj,
        const std::string& key, const std::string& defaultValue = "" );

protected:

    virtual std::unique_ptr<KICAD_CURL_EASY> createCurlEasyObject()
    {

        std::unique_ptr<KICAD_CURL_EASY> aCurl( new KICAD_CURL_EASY() );

        // prepare curl
        aCurl->SetHeader( "Accept", "application/json" );
        aCurl->SetHeader( "Authorization", "Token " + m_source.token );
        aCurl->SetHeader( "Content-Type", "application/json" );

        return aCurl;
    }

    virtual bool ValidateHTTPLibraryEndpoints() { return true; }

    virtual bool syncCategories();

    HTTP_HQ_LIB_SOURCE      m_source;

    //          part.id     part
    // std::map<std::string, HTTP_HQ_PART> m_cachedParts;

    //        part.name               part.id     category.id
    // std::map<std::string, std::tuple<std::string, std::string>> m_cache;


    std::string m_lastError;

    std::vector<HTTP_HQ_CATEGORY> m_categories;

    std::vector<HTTP_HQ_PART> m_parts;

    const std::string http_categories = "/api/chiplet/kicad/cateTree";
    const std::string http_product_details = "/api/chiplet/products/productDetail";
    const std::string http_query_parts = "/api/chiplet/products/kicad/queryPage";

};

#endif //KICAD_HTTP_HQ_CONNECTION_H
