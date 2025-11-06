#include <utils/file_path_generator.h>
#include <utils/copilot_zip_utils.h>
#include <utils/httplib_wrapper.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <iostream>
#include <api/upload_project.h>

using httplib::MultipartFormData;
using httplib::MultipartFormDataItems;
using httplib::SSLClient;
using json = nlohmann::json;

// Upload project to eda.cn, return the file URL if success
std::optional<std::string> upload_project( std::string aProjectDir )
{
    auto output_zip = FILE_PATH_GENERATOR::get().gen_tmp_file( ".zip" );

    if( !COPILOT_ZIP_UTILS::zip( output_zip, aProjectDir ) )
        return {};

    SSLClient cli( "www.eda.cn", 443 );

    MultipartFormDataItems items;

    std::ifstream file( output_zip, std::ios::binary );

    if( file )
    {
        std::string file_content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
        std::string filename = output_zip.substr( output_zip.find_last_of( "/\\" ) + 1 );
        items.push_back( { "file", file_content, filename, "application/zip" } );
    }


    auto res = cli.Post( "/openapi/datastream/upload", items );

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
