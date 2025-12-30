#ifndef PCB_PRIMITIVES_EXPORT_H
#define PCB_PRIMITIVES_EXPORT_H

#include <string>
#include <vector>

enum class KI_SDK_EXEC_RESULT : int;

class PCB_PRIMITIVES_EXPORT
{
public:
    KI_SDK_EXEC_RESULT ExportDSNFile( const std::string& aFileName );

    KI_SDK_EXEC_RESULT ExportGenCAD( const std::string& aFileName, bool aUseAuxOrigin = false, bool aFilpBottomPads = false,
                                     bool aUniquePinName = false, bool aIndividualShapes = false, bool aStoreOriginCoords = false);

    KI_SDK_EXEC_RESULT ExportVRML( const std::string& aFileName );

    KI_SDK_EXEC_RESULT ExportIDFV3( const std::string& aFileName );

    KI_SDK_EXEC_RESULT ExportGerber( const std::string& aFileName );

    KI_SDK_EXEC_RESULT ExportOdbPlusPlus( const std::string& aFileName );
};


#endif