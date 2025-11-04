/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Liu Weijian <lweijian210@gmail.com>
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

#ifndef PCB_COPILOT_INIT_H
#define PCB_COPILOT_INIT_H


#include <pcb_edit_frame.h>

void PCB_EDIT_FRAME::CopilotPanelInit()
{
    m_copilotDsnHdl = std::make_shared<DSN_HDL_T>(
                        [this]( std::string& filename, std::string& base64Data )
                    {
                        return GetDSNFileBase64Data( filename, base64Data );
                    } );
    m_copilotSetJobIdHdl =  std::make_shared<SETJOBID_HDL_T>(
                    [this]( const std::string& job_id )
                    {
                        return SetJobId( job_id );
                    } );
    m_copilotGetJobIdHdl = std::make_shared<GETJOBID_HDL_T>(
                    [this]() -> std::string
                    {
                        return GetJobId();
                    } ) ;
    m_copilotSpecHdl = std::make_shared<SPEC_HDL_T>(
                    [this]( const std::string& data )
                    {
                        return ImportSpectraBase64Data( data );
                    } );


    m_containerCfg = config();
    m_hostVersionInfo = get_copilot_host_version( editor_type() );
    m_hostVersionInfo.chat_id = m_chat_id;
    m_hostVersionInfo.session_id = GetSessionId();

    //freerouting handles
    m_copilotFreeroutingHdl = { m_copilotDsnHdl, m_copilotSetJobIdHdl, m_copilotGetJobIdHdl, m_copilotSpecHdl };
    m_copilotPanel = new COPILOT_WEBVIEW_CONTAINER(
                m_hostVersionInfo, this, add_parameter_to_url( get_webview_chat_path(), m_hostVersionInfo ),
                { m_copilotGlobalContextHdl, m_copilotAgentActionHdl, m_copilotSelectionHdl,
                  m_copilotFreeroutingHdl } ); //m_copilotDsnHdl

    m_copilotContextCache.host_version_info = m_hostVersionInfo;
}

#endif