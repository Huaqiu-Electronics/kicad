#ifndef API_CPP_SDK_RUN_ACTION_H
#define API_CPP_SDK_RUN_ACTION_H

#include <string>
#include "api_cpp_sdk_type.h"
#include "api_cpp_sdk_param.h"
#include <eda_draw_frame.h>
#include <magic_enum.hpp>
#include "api_primitives_drawer.h"

class API_CPP_SDK_RUN_ACTION
{
public:
    virtual void runSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
    {
        return runBaseSdkApi( aFrame, aParams, aOutput );
    }

    void runBaseSdkApi( EDA_DRAW_FRAME* aFrame, CPP_SDK_API_PARAM const& aParams, std::string& aOutput )
    {
        std::string api_name = aParams.api;

        auto api_type = magic_enum::enum_cast<API_CPP_SDK_TYPE>( api_name );

        if( !api_type.has_value() )
        {
            wxLogError( "Failed to enum_cast: %s", api_name );
            return;
        }

        API_PRIMITIVES_DRAWER api_drawer( aFrame );
        try
        {
            switch( *api_type )
            {
                case API_CPP_SDK_TYPE::queryCurrentFrameType:
                {
                    API_FRAME_PARAMS frame_type;
                    api_drawer.GetCurrentFrame( frame_type );
                    nlohmann::json json_obj = frame_type;
                    aOutput = json_obj.dump();
                    break;
                }
                case API_CPP_SDK_TYPE::closeFrame:
                {
                    const auto& params = aParams.parameter.get<API_FRAME_PARAMS>();
                    api_drawer.CloseFrame( params );
                    break;
                }
                case API_CPP_SDK_TYPE::openFrame:
                {
                    const auto& params = aParams.parameter.get<API_FRAME_PARAMS>();
                    api_drawer.OpenFrame( params );
                    break;
                }
                case API_CPP_SDK_TYPE::saveFrame:
                {
                    api_drawer.SaveFrame();
                    break;
                }
                case API_CPP_SDK_TYPE::saveAs:
                {
                    api_drawer.SaveAs();
                    break;
                }
                case API_CPP_SDK_TYPE::pageSetting:
                {
                    api_drawer.OpenPageSettingDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::print:
                {
                    api_drawer.OpenPrintDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::plot:
                {
                    api_drawer.OpenPlotDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::closeCurrentFrame:
                {
                    api_drawer.CloseCurrentFrame();
                    break;
                }
                case API_CPP_SDK_TYPE::find:
                {
                    api_drawer.OpenFindDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::findReplace:
                {
                    api_drawer.OpenFindReplaceDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::deleteTool:
                {
                    api_drawer.DeleteItem();
                    break;
                }
                case API_CPP_SDK_TYPE::selectAll:
                {
                    api_drawer.SelectAllItems();
                    break;
                }
                case API_CPP_SDK_TYPE::unselectAll:
                {
                    api_drawer.UnSelectAllItems();
                    break;
                }
                case API_CPP_SDK_TYPE::propertyPanel:
                {
                    api_drawer.ShowPropertyPanel();
                    break;
                } 
                case API_CPP_SDK_TYPE::searchPanel:
                {
                    api_drawer.ShowSearchPanel();
                    break;
                }
                case API_CPP_SDK_TYPE::zoomInCenter:
                {
                    api_drawer.ZoomInCenter();
                    break;
                } 
                case API_CPP_SDK_TYPE::zoomOutCenter:
                {
                    api_drawer.ZoomOutCenter();
                    break;
                }
                case API_CPP_SDK_TYPE::zoomFitScreen:
                {
                    api_drawer.ZoomFitScreen();
                    break;
                }
                case API_CPP_SDK_TYPE::zoomFitObjects:
                {
                    api_drawer.ZoomFitObjects();
                    break;
                }
                case API_CPP_SDK_TYPE::configurePaths:
                {
                    api_drawer.OpenConfigurePathDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::managerSymbolLibrary:
                {
                    api_drawer.OpenSymbolLibDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::mangerDesignBlockLibarary:
                {
                    api_drawer.OpenDesignBlockLibDlg();
                    break;
                }
                case API_CPP_SDK_TYPE::Preference:
                {
                    api_drawer.OpenPreferencesDlg();
                    break;
                } 
                default: break;
                }
        }
        catch(const std::exception& e)
        {
            wxLogError( "Failed to Find API : %s,exception :  %s", api_name, e.what() );
        }
        return;
    }
};







#endif