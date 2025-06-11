#include <settings/parameters.h>
#include <settings/app_settings.h>
void APP_SETTINGS_BASE::initCopilotSettings()
{
    m_params.emplace_back( new PARAM<bool>( "aui.copilot_panel_show", &copilot_panel_show, true ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.copilot_panel_docked_width", &copilot_panel_docked_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.copilot_panel_float_width", &copilot_panel_float_width, -1 ) );

    m_params.emplace_back(
            new PARAM<int>( "aui.copilot_panel_float_height", &copilot_panel_float_height, -1 ) );
}
