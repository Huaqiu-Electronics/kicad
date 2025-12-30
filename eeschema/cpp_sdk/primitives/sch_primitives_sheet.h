#ifndef SCH_PRIMITIVES_SHEET_H
#define SCH_PRIMITIVES_SHEET_H

#include <string>
#include <vector>
#include <math/vector2d.h>

class SCH_HIERLABEL;
class SCH_SHEET;
enum class KI_SDK_EXEC_RESULT : int;

class SCH_PRIMITIVES_SHEET
{
public:
    KI_SDK_EXEC_RESULT  CreateHierSheet(const VECTOR2I& position, int width, int height, const std::string& title, std::string& aKID);

    KI_SDK_EXEC_RESULT  CreateSheetPin(const std::string& aPinName, std::string& aKID);

    KI_SDK_EXEC_RESULT  GetRootSheetKID(std::string& aKID);

    KI_SDK_EXEC_RESULT  GetRootSheetPathList(std::vector<std::string>& aSheetPathList);

    KI_SDK_EXEC_RESULT  GetSheetPageNumberByPath(const std::string& aPath, std::string& aPageNumber);

    KI_SDK_EXEC_RESULT  GetSheetKIDByPageNumber(const std::string& aPageNumber, std::string& aKID);

    KI_SDK_EXEC_RESULT  GetSheetShownNameByPageNumber(const std::string& aPageNumber, std::string& aName, std::string& aShownName);

    KI_SDK_EXEC_RESULT  GetCurrentSheetLastScreen( std::string& aKID );

    KI_SDK_EXEC_RESULT  GetCurrentSheetLastScreenFileName( std::string& aFileName );


    KI_SDK_EXEC_RESULT  GetAllScreenKIDList(std::vector<std::string>& aKIDList);

    KI_SDK_EXEC_RESULT  GetAllScreensFileNameList(std::vector<std::string>& aFileList);

private:
    SCH_HIERLABEL*  importHierLabel( SCH_SHEET* aSheet );


};





#endif