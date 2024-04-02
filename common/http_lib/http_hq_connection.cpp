/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2023 Andre F. K. Iwers <iwers11@gmail.com>
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

#include <wx/log.h>
#include <fmt/core.h>
#include <wx/translation.h>
#include <ctime>

#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include <wx/base64.h>

#include <kicad_curl/kicad_curl_easy.h>
#include <curl/curl.h>

#include <http_lib/http_hq_connection.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <settings/environment.h>
#include <string_utils.h>
#include <pgm_base.h>
#include <env_vars.h>




// const char* const traceHTTPLib = "KICAD_HTTP_LIB";


HTTP_HQ_CONNECTION::HTTP_HQ_CONNECTION( const HTTP_HQ_LIB_SOURCE& aSource )
: HTTP_LIB_CONNECTION(), m_source( aSource )
{
}


HTTP_HQ_CONNECTION::~HTTP_HQ_CONNECTION()
{
    // Do nothing
}

void HTTP_HQ_CONNECTION::SetHttpSource( const HTTP_HQ_LIB_SOURCE& aSource )
{
    m_source = aSource;
}

std::string HTTP_HQ_CONNECTION::GetFieldsFromSource()
{
    nlohmann::json items;

    if( m_source.params.empty() )
    {
        return "{}";
    }

    for( const auto& pair : m_source.params )
    {
        items[pair.first] = pair.second;
    }

    std::string fields = items.dump();
    return fields;
}


bool HTTP_HQ_CONNECTION::syncCategories()
{
    std::string res = "";

    std::unique_ptr<KICAD_CURL_EASY> curl = createCurlEasyObject();
    curl->SetPostFields( GetFieldsFromSource() );

    curl->SetURL( m_source.root_url + http_categories );

    try
    {
        curl->Perform();

        res = curl->GetBuffer();

        if( !checkServerResponse( curl ) )
        {
            return false;
        }

        nlohmann::json response = nlohmann::json::parse( res );

        // collect the categories in vector
        for( const auto& item : response["result"] )
        {
            HTTP_HQ_CATEGORY category;

            category.id = std::to_string( item["cateId"].get<int>() );
            category.name = item["cateName"].get<std::string>();
            category.parentId = std::to_string( item["parentId"].get<int>() );
            category.displayName = item["cateDisplayName"].get<std::string>();
            category.level = std::to_string( item["level"].get<int>() );

            m_categories.push_back( category );

        }
    }
    catch( const std::exception& e )
    {
        m_lastError += wxString::Format( _( "Error: %s" ) + "\n" + _( "API Response:  %s" ) + "\n",
                                         e.what(), res );

        wxLogTrace( traceHTTPLib,
                    wxT( "syncCategories: Exception occurred while syncing categories: %s" ),
                    m_lastError );

        m_categories.clear();

        return false;
    }

    return true;
}


bool HTTP_HQ_CONNECTION::QueryParts( const std::vector<std::pair<std::string, std::string>>& aFields )
{
    m_parts.clear();
    std::string res = "";

    std::unique_ptr<KICAD_CURL_EASY> curl = createCurlEasyObject();
    curl->SetURL( m_source.root_url + http_query_parts );

    curl->SetPostFields( GetJsonPostFields( aFields ) );

    try
    {
        curl->Perform();

        res = curl->GetBuffer();

        if( !checkServerResponse( curl ) )
        {
            return false;
        }

        nlohmann::json response = nlohmann::json::parse( res );

        if( !response.contains("result") || !response["result"].is_array() )
            return false;

        for( const auto& item : response["result"] )
        {
            if( item.contains( "queryPartVO" ) )
            {
                HTTP_HQ_PART hq_part;
                const auto& part = item["queryPartVO"]["part"];
                hq_part.manufacturerId = part["manufacturer_id"].get<std::string>();
                hq_part.mpn = part["mpn"].get<std::string>();
                hq_part.id = part["component_id"].get<std::string>();
                hq_part.datasheet = part["datasheet"].get<std::string>();
                hq_part.description = part["part_desc"].get<std::string>();
                hq_part.pkg = part["pkg"].get<std::string>();
                
                m_parts.push_back( hq_part );
                // m_parts[hq_part.mpn] = hq_part;
            }
        }
    }
    catch( const std::exception& e )
    {
        m_lastError += wxString::Format( _( "Error: %s" ) + "\n" + _( "API Response:  %s" ) + "\n",
                                         e.what(), res );

        wxLogTrace( traceHTTPLib,
                    wxT( "Exception occurred while query parts: %s" ),
                    m_lastError );

        m_parts.clear();

        return false;
    }

    return true;
}

bool HTTP_HQ_CONNECTION::RequestPartDetails( HTTP_HQ_PART& aPart )
{
    std::string res = "";
    std::unique_ptr<KICAD_CURL_EASY> curl = createCurlEasyObject();
    curl->SetURL( m_source.root_url + http_product_details );

    std::vector<std::pair<std::string, std::string>> fields;
    fields.push_back( std::make_pair( "manufacturer_id", aPart.manufacturerId ) );
    fields.push_back( std::make_pair( "mpn", aPart.mpn ) );

    curl->SetPostFields( GetJsonPostFields( fields ) );

    try
    {
        curl->Perform();

        res = curl->GetBuffer();

        if( !checkServerResponse( curl ) )
        {
            return false;
        }

        nlohmann::json response = nlohmann::json::parse( res );

        if( !response.contains("result") || !response["result"].contains("cadUrlList") )
            return false;
        
        for( const auto& item : response["result"]["cadUrlList"] )
        {
            if( item.contains( "fileUrl" ) )
            {
                std::string type = item["type"].get<std::string>();
                if( type != "symbol" && type != "footprint" )
                    continue;
                std::string url = item["fileUrl"].get<std::string>();;
                aPart.fields[type] = url;
                // GetLibSavePath( type, aPart );
            }
        }
    }
    catch( const std::exception& e )
    {
        m_lastError += wxString::Format( _( "Error: %s" ) + "\n" + _( "API Response:  %s" ) + "\n",
                                         e.what(), res );

        wxLogTrace( traceHTTPLib,
                    wxT( "Exception occurred while query part %s details: %s" ),
                    aPart.mpn, m_lastError );

        m_parts.clear();

        return false;
    }

    return true;
}

wxString HTTP_HQ_CONNECTION::GetLibSavePath( std::string aType, HTTP_HQ_PART& aPart )
{
    wxString packagesPath;
    const ENV_VAR_MAP& vars = Pgm().GetLocalEnvVariables();

    if( std::optional<wxString> v = ENV_VAR::GetVersionedEnvVarValue( vars, wxT( "3RD_PARTY" ) ) )
        packagesPath = *v;

    wxFileName fn( packagesPath, wxS( "" ) );

    wxArrayString words;
    wxStringSplit( aPart.fields[aType], words, wxT( '/' ) );
    wxString filename = words.Last();
    
    if( aType == "symbol" )
    {
        fn.AppendDir( wxS( "symbols" ) );
        fn.AppendDir( wxS( "hq_symbols" ) );
        aPart.symbol_lib_name = filename.ToStdString();  // *.kicad_symbol
    }
    else if( aType == "footprint" )
    {
        fn.AppendDir( wxS( "footprints" ) );
        fn.AppendDir( wxS( "hq_footprints" ) );
        fn.AppendDir( wxString::Format( "%s.pretty", aPart.pretty_name ) );
        aPart.fp_lib_name = filename.ToStdString();   // *.kicad_mod
    }
    
    fn.SetFullName( filename );
    return fn.GetFullPath();
}

bool HTTP_HQ_CONNECTION::DownloadLibs( std::string aType, HTTP_HQ_PART& aPart )
{
    if( aType != "symbol" && aType != "footprint" )
        return false;
    
    wxFileName fn( GetLibSavePath( aType, aPart ) );

    if( !fn.FileExists() )
    {
        std::string res = "";
        std::unique_ptr<KICAD_CURL_EASY> curl = createCurlEasyObject();
        curl->SetURL( aPart.fields[aType] );

        try
        {
            curl->Perform();

            res = curl->GetBuffer();

            if( !checkServerResponse( curl ) )
            {
                return false;
            }
        }
        catch( const std::exception& e )
        {
            m_lastError += wxString::Format( _( "Error: %s" ) + "\n" + _( "API Response:  %s" ) + "\n",
                                            e.what(), res );

            wxLogTrace( traceHTTPLib,
                        wxT( "Download: Exception occurred while download libs files: %s" ),
                        m_lastError );

            return false;
        }

        if( !fn.DirExists() && !fn.Mkdir( wxS_DIR_DEFAULT, 0755 ) )
        {
            wxLogError( wxString::Format( _( "Cannot create hq library path '%s'." ),
                                            fn.GetPath() ) );
            return false;
        }

        if( !fn.IsDirWritable() || ( fn.FileExists() && !fn.IsFileWritable() ) )
            return false;
        
        try
        {
            wxFFileOutputStream fileStream( fn.GetFullPath(), "wb" );

            if( !fileStream.IsOk() || !fileStream.WriteAll( res.c_str(), res.size() ) )
            {
                wxLogTrace( traceHTTPLib, wxT( "Warning: could not save %s" ), fn.GetFullPath() );
            }

            fileStream.Close();

        }
        catch( const std::exception& e )
        {
            wxLogTrace( traceHTTPLib,
                        wxT( "Catch error: could not save %s. IO error %s" ),
                        fn.GetFullPath(), e.what() );
            return false;
        }
    }
    return true;

}

std::string HTTP_HQ_CONNECTION::GetJsonPostFields(
                const std::vector<std::pair<std::string, std::string>>& aFields )
{
    nlohmann::json j_obj;

    for (const auto& field : aFields)
    {
        j_obj[field.first] = field.second;
    }

    std::string postfields = j_obj.dump();
    return postfields;
}







