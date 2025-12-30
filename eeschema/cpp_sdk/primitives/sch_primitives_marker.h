#ifndef SCH_PRIMITIVES_MARKER_H
#define SCH_PRIMITIVES_MARKER_H

#include <string>
#include <math/vector2d.h>
#include <math/box2.h>
#include <wx/string.h>
#include <gal/color4d.h>

enum class KI_SDK_EXEC_RESULT : int;

class SCH_SHEET_PATH;
class SCH_PIN;

class SCH_PRIMITIVES_MARKER
{
public:
    KI_SDK_EXEC_RESULT AddMarkerAtPosition( const VECTOR2I& aPosition ,SCH_SHEET_PATH& aSheetPath, SCH_PIN* aPin, std::string& aKID);

    KI_SDK_EXEC_RESULT ClearMarkerAtPosition( const VECTOR2I& aPosition, SCH_SHEET_PATH& aSheetPath );

    KI_SDK_EXEC_RESULT GetAllMarkerKIID( std::vector<std::string>& aKIDList );

    KI_SDK_EXEC_RESULT FindMarkerKIDByPosition( const VECTOR2I& aPosition, std::string& aKID );
};


#endif