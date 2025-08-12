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

#ifndef QUOTE_CONTROLLER_H
#define QUOTE_CONTROLLER_H

#include <copilot_global.h>
#include <memory>
#include <optional>
#include "pcb_quote_response.h"

namespace httplib
{
class Client;
class SSLClient;
} // namespace httplib


#if !defined( IS_RC_BUILD )
using QuoteClient = httplib::SSLClient;
#else
using QuoteClient = httplib::Client;
#endif


struct PCB_ORDER;
struct PCB_SMT_ORDER;

struct PCB_SMT_QUOTE_FILE_PATHS
{
    std::string pcb_file_path;
    std::string bom_file_path;
    std::string coord_file_path;
};

class COPILOT_API QUOTE_CONTROLLER
{
public:
    QUOTE_CONTROLLER();

    ~QUOTE_CONTROLLER();

    std::optional<PCB_QUOTE_RESPONSE> pcb_quote( PCB_ORDER const& form, std::string const& attachment_file_path );

    std::optional<std::string> pcb_smt_quote( PCB_SMT_ORDER const& form, PCB_SMT_QUOTE_FILE_PATHS const& file_paths );


private:
    std::unique_ptr<QuoteClient> _client;
};

#endif
