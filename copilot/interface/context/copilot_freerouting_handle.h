/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Liu Weijian <lweijian210@gmail.com>
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

#ifndef COPILOT_FREEROUTING_HANDLE_H
#define COPILOT_FREEROUTING_HANDLE_H

#include <functional>
#include <memory>
#include <string>




//freerouting
using DSN_HDL_T = std::function<void(std::string&,std::string&)>;
using COPILOT_DSN_OWNED_HDL = std::shared_ptr<std::function<void(std::string&,std::string&)>>;

using SETJOBID_HDL_T = std::function<void(const std::string&)>;
using COPILOT_SETJOBID_OWNER_HDL = std::shared_ptr<std::function<void(const std::string&)>>;

using GETJOBID_HDL_T = std::function<std::string()>;
using COPILOT_GETJOBID_OWNED_HDL = std::shared_ptr<std::function<std::string()>>;

using SPEC_HDL_T = std::function<void(const std::string&)>;
using COPILOT_SPEC_OWNER_HDL = std::shared_ptr<std::function<void(const std::string&)>>;

#endif