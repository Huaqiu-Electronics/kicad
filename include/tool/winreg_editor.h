/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2023 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2023 KiCad Developers, see AUTHORS.txt for contributors.
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


#ifndef WINREG_EDITOR_H
#define WINREG_EDITOR_H

#include "WinReg.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>
#include <wx/msgdlg.h>


using std::pair;
using std::vector;
using std::wcout;
using std::wstring;

using winreg::RegException;
using winreg::RegExpected;
using winreg::RegKey;


static const std::wstring kSubKeyL0 = L"SOFTWARE\\Policies\\Microsoft";
static const std::wstring kSubKeyL1 = L"SOFTWARE\\Policies\\Microsoft\\InputMethod";
static const std::wstring kSubKeyL2 = L"SOFTWARE\\Policies\\Microsoft\\InputMethod\\Settings";
static const std::wstring kSubKeyL3 = L"SOFTWARE\\Policies\\Microsoft\\InputMethod\\Settings\\CHS";
const wstring             kConfigureImeVersion = L"ConfigureImeVersion";

static const std::wstring kKey0 = L"InputMethod";
static const std::wstring kKey1 = L"Settings";
static const std::wstring kKey2 = L"CHS";


class WINREG_EDITOR
{
    static auto doEnable( bool aEnable )
    {
        for( const auto& it : { std::pair( kSubKeyL0, kKey0 ), std::pair( kSubKeyL1, kKey1 ),
                                std::pair( kSubKeyL2, kKey2 ) } )
        {
            RegKey key;
            key.TryOpen( HKEY_CURRENT_USER, it.first );
            key.TryCreate( key.Get(), it.second );
        }

        RegKey key;

        if( !key.TryOpen( HKEY_CURRENT_USER, kSubKeyL3 ).IsOk() )
            return false;

        if( auto res = key.TrySetDwordValue( kConfigureImeVersion, aEnable ? 1 : 0 ); !res.IsOk() )
            return false;

        return true;
    }

public:
    // Computer\\HKEY_CURRENT_USER\\SOFTWARE\\Policies\\Microsoft\\InputMethod\\Settings\\CHS
    // ConfigureImeVersion
    static auto IsMicrosoftPinYinCompatibilityModeEnabled() -> bool
    {
        RegKey key;

        if( !key.TryOpen( HKEY_CURRENT_USER, kSubKeyL3 ).IsOk() )
            return false;

        auto res = key.TryGetDwordValue( kConfigureImeVersion );

        if( !res.IsValid() || res.GetValue() != 1 )
            return false;

        return true;
    }

    static auto SetMicrosoftPinYinCompatibilityMode( bool aEnable )
    {
        if( doEnable( aEnable ) )
        {
            wxMessageBox( wxString::Format( _( "成功%s微软拼音输入法兼容模式" ),
                                            !aEnable ? wxT( "禁用" ) : wxT( "启用" ) ),
                          _( "提示" ), wxOK | wxICON_INFORMATION );
        }
        else
        {
            wxMessageBox( wxString::Format( _( "%s微软拼音输入法兼容模式失败" ),
                                            !aEnable ? wxT( "禁用" ) : wxT( "启用" ) ),
                          _( "错误" ), wxOK | wxICON_ERROR );
        }
    }


private:
};

#endif
