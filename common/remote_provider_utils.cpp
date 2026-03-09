/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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

#include <remote_provider_utils.h>

#include <wx/intl.h>
#include <wx/uri.h>


wxString UrlEncode( const wxString& aValue )
{
    wxString encoded;

    for( wxUniChar ch : aValue )
    {
        if( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) || ( ch >= '0' && ch <= '9' )
            || ch == '-' || ch == '_' || ch == '.' || ch == '~' )
        {
            encoded.Append( ch );
        }
        else
        {
            encoded.Append( wxString::Format( wxS( "%%%02X" ), static_cast<unsigned int>( ch ) ) );
        }
    }

    return encoded;
}


wxString RemoteProviderJsonString( const nlohmann::json& aObject, const char* aKey )
{
    auto it = aObject.find( aKey );

    if( it != aObject.end() && it->is_string() )
        return wxString::FromUTF8( it->get_ref<const std::string&>().c_str() );

    return wxString();
}


bool IsLoopbackHost( const wxString& aHost )
{
    wxString host = aHost.Lower();

    if( host.StartsWith( wxS( "[" ) ) && host.EndsWith( wxS( "]" ) ) )
    {
        host.RemoveLast();
        host.Remove( 0, 1 );
    }

    return host == wxS( "localhost" ) || host == wxS( "127.0.0.1" ) || host == wxS( "::1" );
}


bool ValidateRemoteUrlSecurity( const wxString& aUrl, bool aAllowInsecureLocalhost,
                                wxString& aError, const wxString& aLabel )
{
    if( aUrl.IsEmpty() )
        return true;

    wxURI    uri( aUrl );
    wxString scheme = uri.GetScheme().Lower();

    if( scheme == wxS( "https" ) )
        return true;

    if( scheme == wxS( "http" ) && aAllowInsecureLocalhost && IsLoopbackHost( uri.GetServer() ) )
        return true;

    aError = wxString::Format(
            _( "%s must use HTTPS unless allow_insecure_localhost is enabled for a loopback URL." ),
            aLabel );
    return false;
}


wxString NormalizedUrlOrigin( const wxString& aUrl )
{
    if( aUrl.IsEmpty() )
        return wxString();

    wxURI uri( aUrl );

    if( !uri.HasScheme() || !uri.HasServer() )
        return wxString();

    wxString scheme = uri.GetScheme().Lower();
    wxString host = uri.GetServer().Lower();
    wxString port = uri.GetPort();

    if( port.IsEmpty() )
    {
        if( scheme == wxS( "https" ) )
            port = wxS( "443" );
        else if( scheme == wxS( "http" ) )
            port = wxS( "80" );
    }

    return scheme + wxS( "://" ) + host + wxS( ":" ) + port;
}


void COLLECTING_JSON_ERROR_HANDLER::error( const nlohmann::json::json_pointer& aPointer,
                                           const nlohmann::json&               aInstance,
                                           const std::string&                  aMessage )
{
    wxUnusedVar( aInstance );
    m_errors.emplace_back( wxString::Format( wxS( "%s: %s" ),
                                             wxString::FromUTF8( aPointer.to_string().c_str() ),
                                             wxString::FromUTF8( aMessage.c_str() ) ) );
}


wxString COLLECTING_JSON_ERROR_HANDLER::FirstError() const
{
    if( m_errors.empty() )
        return wxString();

    return m_errors.front();
}
