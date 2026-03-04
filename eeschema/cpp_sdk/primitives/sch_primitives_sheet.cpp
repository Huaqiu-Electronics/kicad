#include "sch_primitives_sheet.h"
#include <cpp_sdk/utils/ki_sdk_sch_utils.h>
#include <sch_sheet.h>
#include <sch_sheet_pin.h>
#include <algorithm>

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::CreateHierSheet(const VECTOR2I& position, int width, int height, const std::string& title, std::string& aKID)
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC* schematic = nullptr;
    GetSchEditorFrame(frame);
    GetSchematic(schematic);

    if(nullptr == frame || nullptr == schematic)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }

    wxString    file_name = wxString::FromUTF8(title);
    wxString    full_name = wxString::FromUTF8(title + ".kicad_sch");


    SCH_SHEET* sheet = new SCH_SHEET( frame->GetCurrentSheet().Last(), position, VECTOR2I(width, height));
    sheet->SetScreen(nullptr);
    // sheet->GetFields()[SHEETNAME].SetText( file_name);
    // sheet->GetFields()[SHEETFILENAME].SetText(full_name);

    sheet->SetBorderWidth( schIUScale.MilsToIU( frame->eeconfig()->m_Drawing.default_line_thickness ) );
    sheet->SetBorderColor( frame->eeconfig()->m_Drawing.default_sheet_border_color );
    sheet->SetBackgroundColor( frame->eeconfig()->m_Drawing.default_sheet_background_color );


    SCH_SHEET_LIST hierarchy = frame->Schematic().Hierarchy();
    SCH_SHEET_PATH instance = frame->GetCurrentSheet();
    instance.push_back(sheet);
    wxString page_number;
    page_number.Printf(wxT("%d"),  static_cast<int>(hierarchy.size()) + 2 ) ;
    instance.SetPageNumber(page_number);

    SCH_SCREEN*    current_screen = frame->GetCurrentSheet().LastScreen();
    wxFileName current_screen_fileName = current_screen->GetFileName();
    wxFileName     screen_fileName( full_name );
    screen_fileName.Normalize(  FN_NORMALIZE_FLAGS | wxPATH_NORM_ENV_VARS,
                                    current_screen_fileName.GetPath());


    wxString newAbsolute_filename = screen_fileName.GetFullPath();

    frame->AllowCaseSensitiveFileNameClashes( sheet->GetFileName(),
                                                         newAbsolute_filename );
    if(wxFileExists( newAbsolute_filename ))
    {
        SCH_SHEET_PATH& current_sheet = frame->GetCurrentSheet();
        bool load_mark = frame->LoadSheetFromFile( sheet, &current_sheet, newAbsolute_filename, false, true );
        bool check_mark =  frame->CheckSheetForRecursion( sheet, &current_sheet );
        if(load_mark == false || check_mark == true)
        {
            delete sheet;
            sheet = nullptr;
            return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
        }
    }else
    {
        frame->InitSheet(sheet,newAbsolute_filename);
    }
    

    SCH_SHEET_LIST repaired_list;
    repaired_list.BuildSheetList( &frame->Schematic().Root(), true );


    frame->TestDanglingEnds();

    // Refresh all sheets in case ordering changed.
    for( SCH_ITEM* item : frame->GetScreen()->Items().OfType( SCH_SHEET_T ) )
        frame->UpdateItem( item );


                // We need to manually add the sheet to the screen otherwise annotation will not be able to find
                // the sheet and its symbols to annotate.
    frame->AddToScreen(sheet, frame->GetScreen());


    schematic->RefreshHierarchy();

    frame->UpdateHierarchyNavigator();

    aKID = sheet->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::CreateSheetPin(const std::string& aPinName, std::string& aKID)
{
    SCH_EDIT_FRAME* frame = nullptr;
    SCHEMATIC* schematic = nullptr;
    GetSchEditorFrame(frame);
    GetSchematic(schematic);

    if(nullptr == frame || nullptr == schematic)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FRAME_ERROR;
    }


    wxString   data_sheet = wxString::FromUTF8(aPinName);
    SCH_SCREEN* current_screen = frame->GetScreen();

    std::vector<SCH_ITEM*> temp_sub_sheets;
    current_screen->GetSheets( &temp_sub_sheets );
    SCH_SHEET*  sheet = nullptr;

    SCH_HIERLABEL* label = nullptr;
    for( SCH_ITEM* sheet_item : temp_sub_sheets )
    {
        SCH_SHEET*  sheet_temp = static_cast<SCH_SHEET*>(sheet_item);
        wxString item_name = sheet_temp->GetName();
        if(item_name.CmpNoCase(data_sheet) == 0)
        {
            sheet = sheet_temp;
            SCH_HIERLABEL*  hier_label = importHierLabel(sheet_temp);
            if(hier_label != nullptr)
            {
                label = hier_label;
                break;
            }
           
        }
    }
    if(sheet == nullptr || label == nullptr)
    {
        wxLogError("Failed to find sheet %s in current screen.", data_sheet);
        return KI_SDK_EXEC_RESULT::KI_SDK_NOT_FOUND;
    }


    SCHEMATIC_SETTINGS& settings = sheet->Schematic()->Settings();
    SCH_SHEET_PIN*      pin = new SCH_SHEET_PIN( sheet );

    VECTOR2I pos = sheet->GetRotationCenter();

    pin->SetFlags( IS_NEW  );
    pin->SetText( std::to_string( sheet->GetPins().size() + 1 ) );
    pin->SetTextSize( VECTOR2I( settings.m_DefaultTextSize, settings.m_DefaultTextSize ) );
    pin->SetPosition( pos );
    pin->ClearSelected();
    pin->SetText( label->GetText() );
    pin->SetShape( label->GetShape() );


    if( pin->IsConnectable() )
        frame->AutoRotateItem( frame->GetScreen(), pin );

    aKID = pin->m_Uuid.AsStdString();

    sheet->AddPin(  (SCH_SHEET_PIN*)  pin );


    pin->AutoplaceFields(frame->GetScreen(), AUTOPLACE_AUTO);


    frame->UpdateItem(sheet);
    
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}


SCH_HIERLABEL* SCH_PRIMITIVES_SHEET::importHierLabel( SCH_SHEET* aSheet )
{
    if( !aSheet->GetScreen() )
        return nullptr;

    std::vector<SCH_HIERLABEL*> labels;
    for( EDA_ITEM* item : aSheet->GetScreen()->Items().OfType( SCH_HIER_LABEL_T ) )
    {
        SCH_HIERLABEL* label = static_cast<SCH_HIERLABEL*>( item );
        labels.push_back( label );
    }

    std::sort( labels.begin(), labels.end(),
               []( const SCH_HIERLABEL* label1, const SCH_HIERLABEL* label2 )
               {
                   return StrNumCmp( label1->GetText(), label2->GetText(), true ) < 0;
               } );

    for( auto& label : labels )
    {
        if( !aSheet->HasPin( label->GetText() ) )
            return label;
    }

    return nullptr;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetRootSheetKID(std::string& aKID)
{
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);
    if(schematic == nullptr)
    {
        aKID = nullptr;
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aKID = schematic->Root().m_Uuid.AsStdString();
    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


/*
原理图的层次图说明：
打开一个原理图.kicad_sch后，会自动生成一个根sheet，在原理图模块通过，schematic->Root()可以获取到对应的sheet*指针；
通过schematic->Hierarchy(),可以获取原理图中，所有的层次级别的sheet，返回值是一个SCH_SHEET_LIST， 这是一个std::vector<SCH_SHEET_PATH>
这个std::vector中的每一个值，对应一个sch_sheet_path，也就是对应的sheet的路径，注意，通过同一个sheet可以在SCH_SHEET_LIST中多次出现，但是sch_sheet_path是惟一的；
sch_sheet_path用来描述sheet在Hierarchy()中的位置信息，
通过sch_sheet_path的PathAsString()方法，可以发现，这是把sheet的各级父对象的m_uuid拼接而成，
也就是说，sch_sheet_path中，有一个变量，m_sheets, 类型是std::vector<SCH_SHEET*>,存放其各个父对象的sheet*指针；
*/

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetRootSheetPathList(std::vector<std::string>& aSheetPathList)
{
    aSheetPathList.clear();
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);
    if(schematic == nullptr)
    {

        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for(SCH_SHEET_PATH& sheet_path : schematic->Hierarchy())
    {
        wxString path = sheet_path.PathAsString();
        aSheetPathList.emplace_back(path.ToStdString());
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT  SCH_PRIMITIVES_SHEET::GetSheetPageNumberByPath(const std::string& aPath, std::string& aPageNumber)
{
    aPageNumber = "";
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {

        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for( SCH_SHEET_PATH& sheet_path : schematic->Hierarchy() )
    {
        if( aPath == sheet_path.PathAsString().ToStdString() )
        {
            aPageNumber = sheet_path.GetPageNumber();
            break;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetSheetKIDByPageNumber(const std::string& aPageNumber, std::string& aKID)
{
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for( SCH_SHEET_PATH& sheet_path : schematic->Hierarchy() )
    {
        if( aPageNumber == sheet_path.GetPageNumber() )
        {
            aKID = sheet_path.Last()->m_Uuid.AsStdString();
            break;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetSheetShownNameByPageNumber(const std::string& aPageNumber,std::string& aName, std::string& aShownName)
{
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for( SCH_SHEET_PATH& sheet_path : schematic->Hierarchy() )
    {
        if( aPageNumber == sheet_path.GetPageNumber() )
        {
            aShownName = sheet_path.Last()->GetShownName(false).ToStdString();   // showName -> label shown on screen 
            aName = sheet_path.Last()->GetName().ToStdString();  //name-> Untitled Sheet
            // aName = sheet_path.Last()->GetFileName().ToStdString();   fileName -> Untitiled.kicad_sch
            break;
        }
    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetCurrentSheetLastScreen( std::string& aKID )
{
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aKID = schematic->CurrentSheet().Last()->GetScreen()->m_Uuid.AsStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;

}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetCurrentSheetLastScreenFileName( std::string& aFileName )
{
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    aFileName = schematic->CurrentSheet().Last()->GetScreen()->GetFileName().ToStdString();

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}

KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetAllScreenKIDList(std::vector<std::string>& aKIDList)
{
    aKIDList.clear();
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for(auto& sheet_path : schematic->Hierarchy())
    {
        SCH_SCREEN* screen = sheet_path.LastScreen();
        std::string screen_uuid = screen->m_Uuid.AsStdString();

        auto it = find_if(aKIDList.begin(), aKIDList.end(), 
                                                        [ &screen_uuid ]( const std::string& value )
                                                        {
                                                            return value == screen_uuid;              
                                                        });

        if(it == aKIDList.end())
        {
            aKIDList.emplace_back(screen_uuid);
        }

    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}


KI_SDK_EXEC_RESULT SCH_PRIMITIVES_SHEET::GetAllScreensFileNameList(std::vector<std::string>& aFileList)
{
    aFileList.clear();
    SCHEMATIC* schematic = nullptr;
    GetSchematic(schematic);

    if(schematic == nullptr)
    {
        return KI_SDK_EXEC_RESULT::KI_SDK_FAILURE;
    }

    for(auto& sheet_path : schematic->Hierarchy())
    {
        SCH_SCREEN* screen = sheet_path.LastScreen();
        std::string screen_filename = screen->GetFileName().ToStdString();

        auto it = find_if(aFileList.begin(), aFileList.end(), 
                                                        [ &screen_filename ]( const std::string& value )
                                                        {
                                                            return value == screen_filename;              
                                                        });

        if(it == aFileList.end())
        {
            aFileList.emplace_back(screen_filename);
        }

    }

    return KI_SDK_EXEC_RESULT::KI_SDK_SUCCESS;
}