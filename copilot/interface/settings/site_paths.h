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

#ifndef SITE_PATHS_H
#define SITE_PATHS_H


#include <nlohmann/json.hpp>

struct SITE_PATHS
{
    std::string library = "/library";

    std::string add_component = "/addSymbolDevice";
    std::string add_design_block = "/addFragment";

    std::string add_component_type = "/addComponentType";
    std::string add_design_block_type = "/addDesignBlockType";

    std::string pcb_quote = "/openapi/api/hqpcb/External/fileQuote";
    std::string smt_quote = "/openapi/pcba/saveDfmTmpOrder";
    std::string news_url = "/news/page/bottomNotice/index.html";

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( SITE_PATHS, library, add_component, add_design_block, add_component_type,
                                    add_design_block_type, pcb_quote, smt_quote, news_url )
};

#endif
