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

#include "panel_add_component.h"
#include "fmt/format.h"
#include "webview/internal/webview_internal_cmd.h"
#include "webview/internal/webview_internal_cmd_type.h"

#include <webview/webview_container.h>
#include <passive_action/passive_action_container.h>
#include <webview_library/webview_library_url.h>
#include <magic_enum.hpp>
#include <webview/webview_controller.h>
#include <webview_library/webview_library_url.h>
PANEL_ADD_COMPONENT::PANEL_ADD_COMPONENT( wxDialog* parent, std::optional<std::string> url ) :
        WEBVIEW_CONTAINER( parent, std::move( url ), {}, "adding_online_library" ), _parent( parent )
{
}

PANEL_ADD_COMPONENT::~PANEL_ADD_COMPONENT()
{
}

void PANEL_ADD_COMPONENT::HandleScriptMessage( std::string const& msg )
{
    try
    {
        const auto act_container = nlohmann::json::parse( msg ).get<PASSIVE_ACTION_CONTAINER>();
        switch( act_container.category )
        {
        case INVALID: break;
        case PA_WEB_HOST:
        {
            const auto cmd = act_container.data.get<WEB_HOST_INTERNAL_CMD>();
            auto       t = magic_enum::enum_cast<WEB_HOST_INTERNAL_CMD_TYPE>( cmd.type );

            if( !t.has_value() )
            {
                wxLogDebug( "Invalid message received: %s", msg );
                return;
            }

            switch( *t )
            {
            case WEB_HOST_INTERNAL_CMD_TYPE::finish_adding_design_block:
            case WEB_HOST_INTERNAL_CMD_TYPE::finish_adding_component:
            {
                WEBVIEW_CONTROLLER::Get().RunScript( fmt::format( "{}();", magic_enum::enum_name( *t ) ),
                                                     kSymbolChooserUrl );
                _parent->EndModal( wxID_OK );
                break;
            }

            default:
            {
                wxLogDebug( "Unknown message received: %s", msg );
            }
            }
            break;
        }
        case PA_AGENT: break;
        }
    }
    catch( const std::exception& e )
    {
        wxLogDebug( "Invalid message received: %s , %s", msg, e.what() );
    }
}