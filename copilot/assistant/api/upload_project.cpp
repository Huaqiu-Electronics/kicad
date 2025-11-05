#include <utils/file_path_generator.h>
#include <utils/copilot_zip_utils.h>
#include <utils/httplib_wrapper.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <optional>
#include <iostream>

using httplib::MultipartFormData;
using httplib::MultipartFormDataItems;
using httplib::SSLClient;
using json = nlohmann::json;

// Upload project to eda.cn, return the file URL if success
std::optional<std::string> upload_project( std::string aProjectDir )
{
    // Step 1: Create temporary ZIP file
    auto output_zip = FILE_PATH_GENERATOR::get().gen_tmp_file( ".zip" );

    if( !COPILOT_ZIP_UTILS::zip( output_zip, aProjectDir ) )
        return {};

    // Step 2: Prepare HTTPS client
    SSLClient cli( "www.eda.cn", 443 );
    cli.enable_server_certificate_verification( false ); // Disable if using self-signed certs

    // Step 3: Prepare multipart form data
    MultipartFormDataItems items = { {
            "file",           // form field name
            "",               // content (will be read from file)
            output_zip,       // file path
            "application/zip" // content type
    } };

    // Step 4: Add headers
    httplib::Headers headers = { { "Cookie", "JSESSIONID=2B37FA222544EBBB6D502AEC9BF7F419" } };

    // Step 5: POST upload
    auto res = cli.Post( "/openapi/datastream/upload", headers, items );

    if( !res )
    {
        std::cerr << "Upload failed: " << httplib::to_string( res.error() ) << std::endl;
        return {};
    }

    if( res->status != 200 )
    {
        std::cerr << "HTTP " << res->status << " upload failed: " << res->body << std::endl;
        return {};
    }

    try
    {
        // Step 6: Parse JSON response
        auto j = json::parse( res->body );

        if( j.contains( "code" ) && j["code"] == 200000 && j.contains( "result" ) && j["result"].contains( "url" ) )
        {
            std::string file_url = j["result"]["url"];
            std::cout << "Upload successful: " << file_url << std::endl;
            return file_url;
        }
        else
        {
            std::cerr << "Unexpected JSON structure: " << j.dump( 2 ) << std::endl;
        }
    }
    catch( const std::exception& e )
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    return {};
}
