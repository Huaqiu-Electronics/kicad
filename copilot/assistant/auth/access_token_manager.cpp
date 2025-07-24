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

#include  <auth/access_token_manager.h>

ACCESS_TOKEN_MANAGER::~ACCESS_TOKEN_MANAGER()
{
    
}

ACCESS_TOKEN_MANAGER& ACCESS_TOKEN_MANAGER::Get()
{
    static ACCESS_TOKEN_MANAGER instance;
    return instance;
    
}

std::optional<ACCESS_TOKEN> ACCESS_TOKEN_MANAGER::GetAccessToken()
{
    
    return  {};
}

void ACCESS_TOKEN_MANAGER::SetAccessToken(ACCESS_TOKEN const& token)
{
    
}

ACCESS_TOKEN_MANAGER::ACCESS_TOKEN_MANAGER()
{
    
}

bool ACCESS_TOKEN_MANAGER::LoadAccessToken()
{
    return  {};
}

void ACCESS_TOKEN_MANAGER::SaveAccessToken()
{
    
};