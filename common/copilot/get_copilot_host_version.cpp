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

#include <get_copilot_host_version.h>
#include <pgm_base.h>
#include <magic_enum.hpp>

#include "get_kicad_version_info.h"
#include "host_version_info.h"


HOST_VERSION_INFO get_copilot_host_version( KICAD_COPILOT_EDITORS editor,
                                            std::string const&    aSessionId )
{
    const auto lang_id = Pgm().GetSelectedLanguageIdentifier();

    wxString localeStr;

    if( lang_id == wxLANGUAGE_DEFAULT )
    {
        wxLocale sysLocale;
        sysLocale.Init();                         // use system default
        localeStr = sysLocale.GetCanonicalName(); // e.g., "zh_CN"
    }
    else
    {
        const wxLanguageInfo* info = wxLocale::GetLanguageInfo( lang_id );
        if( info )
            localeStr = info->CanonicalName; // e.g., "en_US"
        else
            localeStr = "zh"; // fallback if no info
    }

    // Extract two-letter language code
    wxString langCode = localeStr.BeforeFirst( '_' );
    if( langCode.IsEmpty() )
        langCode = "zh"; // fallback again if split fails

    return HOST_VERSION_INFO{ { "kicad_desktop", magic_enum::enum_name( editor ).data(), aSessionId,
                                langCode.ToStdString() },
                              get_kicad_version_info() };
}
