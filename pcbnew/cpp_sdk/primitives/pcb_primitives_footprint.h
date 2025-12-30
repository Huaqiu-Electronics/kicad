#ifndef PCB_PRIMITIVES_FOOTPRINT_H
#define PCB_PRIMITIVES_FOOTPRINT_H

#include <math/vector2d.h>
#include <string>
#include <vector>

class EDA_ANGLE;
enum class FLIP_DIRECTION;
enum class KI_SDK_EXEC_RESULT : int;

class PCB_PRIMITIVES_FOOTPRINT
{
public:
    KI_SDK_EXEC_RESULT CreateFootprintLibraryInProject( const std::string& aLibrary );

    KI_SDK_EXEC_RESULT CreateNewFootprintInProject( const int& attrib, const std::string& aLibraryName, const std::string& aFootprintName );
    
    KI_SDK_EXEC_RESULT PlaceFootprint( const std::string& libName, const std::string& fpName, const VECTOR2I& position, EDA_ANGLE& angle,
                                       std::string& aKID );

    KI_SDK_EXEC_RESULT IsFootprintLibraryExists( const std::string& aLibraryName, bool& isExists, bool  aIsGlobal = false);

    KI_SDK_EXEC_RESULT GetAllFootprintKIID( std::vector<std::string>& allKID );

    KI_SDK_EXEC_RESULT GetFootprintPosition( const std::string& aKID, VECTOR2I& position );

    KI_SDK_EXEC_RESULT GetFootprintReference( const std::string& aKID, std::string& reference );

    KI_SDK_EXEC_RESULT GetFootprintFPID( const std::string& aKID, std::string& fPID );

    KI_SDK_EXEC_RESULT GetKIIDByReference( const std::string& reference, std::string& aKID );

    KI_SDK_EXEC_RESULT GetFootprintValue( const std::string& aKID, std::string& value );

    KI_SDK_EXEC_RESULT GetFootprintFieldText( const std::string& aKID, const std::string& aFieldName, std::string& text );

    KI_SDK_EXEC_RESULT GetFootprintAllPads( const std::string& aKID, std::vector<std::string>& padList );

    KI_SDK_EXEC_RESULT GetFootprintLayerID( const std::string& aKID, PCB_LAYER_ID& layer );

    KI_SDK_EXEC_RESULT SetFootprintPosition( const std::string& aKID, VECTOR2I position );

    KI_SDK_EXEC_RESULT SetFootprintReference( const std::string& aKID, const std::string& reference );

    KI_SDK_EXEC_RESULT SetFootprintValue( const std::string& aKID, const std::string& value );

    KI_SDK_EXEC_RESULT AddFootprintField( const std::string& aKID, const std::string& aName, const std::string& text );

    KI_SDK_EXEC_RESULT SetFootprintFieldText( const std::string& aKID, const std::string& aFieldName,
                                              const std::string& text );

    KI_SDK_EXEC_RESULT FlipFootprint( const std::string& aKID, const VECTOR2I& aCentre, FLIP_DIRECTION& aFlipDirection );

    KI_SDK_EXEC_RESULT SetFootprintOrientation( const std::string& aKID, double angle );

    KI_SDK_EXEC_RESULT MoveFootprintByKIID( const std::string& aKID, const VECTOR2I& offset );

    KI_SDK_EXEC_RESULT RotateFootprintByKIID( const std::string& aKID, double degree );

    KI_SDK_EXEC_RESULT GetAllPadKIID( const std::string& aKID, std::vector<std::string>& padList );
};


#endif