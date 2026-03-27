#ifndef SCH_PRIMITIVES_LIB_SYMBOL_H
#define SCH_PRIMITIVES_LIB_SYMBOL_H


#include <string>
#include <vector>
#include <map>
#include <math/vector2d.h>
#include <math/box2.h>
#include <wx/string.h>
#include <map>

enum class KI_SDK_EXEC_RESULT : int;

enum class PIN_ORIENTATION;

enum class ELECTRICAL_PINTYPE;

enum class GRAPHIC_PINSHAPE;

class SCH_PRIMTIVES_LIB_SYMBOL
{
public:
    KI_SDK_EXEC_RESULT CreateLibSymbolLibrary( const std::string& aLibraryName );

    KI_SDK_EXEC_RESULT CreateEmptyLibSymbol( const std::string& aLibraryName, const std::string& aSymbolName,
                                          const std::string& aReference = "U" );

    KI_SDK_EXEC_RESULT CreateLibSymbolPin( const std::string& aPinName, const std::string& aPinNumber,
                                        const VECTOR2I& aPosition, PIN_ORIENTATION aOrientation,
                                        ELECTRICAL_PINTYPE aElectrial, GRAPHIC_PINSHAPE aShape, int aLength = 100,
                                        int aNameTextSize = 50, int aNumberTextSize = 50 );

    KI_SDK_EXEC_RESULT GetLibSymbolName( std::string& aName );

    KI_SDK_EXEC_RESULT GetLibSymbolLibraryName( std::string& aLibraryName );

    KI_SDK_EXEC_RESULT GetAllLibSymbolPinsKIID( std::vector<std::string>& allKIDList );

    KI_SDK_EXEC_RESULT GetLibSymbolPinName( const std::string& aKID, std::string& aPinName );

    KI_SDK_EXEC_RESULT GetLibSymbolPinNumber( const std::string& aKID, std::string& aPinNumber );

    KI_SDK_EXEC_RESULT GetLibSymbolPinElectricalType( const std::string& aKID, ELECTRICAL_PINTYPE& aElectrial );

    KI_SDK_EXEC_RESULT GetLibSymbolPinGraphicStyle( const std::string& aKID, GRAPHIC_PINSHAPE& aStyle );

    KI_SDK_EXEC_RESULT GetLibSymbolPinPosition( const std::string& aKID, VECTOR2I& aPosition );

    KI_SDK_EXEC_RESULT GetLibSymbolPinOrientation( const std::string& aKID, PIN_ORIENTATION& aOrientation );

    KI_SDK_EXEC_RESULT GetLibSymbolPinLength( const std::string& aKID, int& aLength );

    KI_SDK_EXEC_RESULT GetLibSymbolPinNameTextSize( const std::string& aKID, int& aSize );

    KI_SDK_EXEC_RESULT GetLibSymbolPinNumberTextSize( const std::string& aKID, int& aSize );

    KI_SDK_EXEC_RESULT SetLibSymbolPinName( const std::string& aKID, const std::string& aName );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNumber( const std::string& aKID, const std::string& aNumber );

    KI_SDK_EXEC_RESULT SetLibSymbolPinElectricalType( const std::string& aKID, ELECTRICAL_PINTYPE aType );

    KI_SDK_EXEC_RESULT SetLibSymbolPinGraphicStyle( const std::string& aKID, GRAPHIC_PINSHAPE aShape );

    KI_SDK_EXEC_RESULT SetLibSymbolPinPosition( const std::string& aKID, const VECTOR2I& aPosition );

    KI_SDK_EXEC_RESULT SetLibSymbolPinOrientation( const std::string& aKID, PIN_ORIENTATION aOrientation );

    KI_SDK_EXEC_RESULT SetLibSymbolPinLength( const std::string& aKID, int aLength );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNameTextSize( const std::string& aKID, int aSize );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNumberTextSize( const std::string& aKID, int aSize );

    KI_SDK_EXEC_RESULT RotateLibSymbolPin( const std::string& aKID, const bool& aRotateCCW = true);

    KI_SDK_EXEC_RESULT HorizontalLibSymbolPin( const std::string& aKID );

    KI_SDK_EXEC_RESULT VerticalLibSymbolPin( const std::string& aKID );

    KI_SDK_EXEC_RESULT RemoveLibSymbolPin( const std::string& aKID );

    KI_SDK_EXEC_RESULT UpdateLibSymbol( wxString* aOldName = nullptr);

    KI_SDK_EXEC_RESULT GetLibSymbolUnitCount( int& aCount );

    KI_SDK_EXEC_RESULT GetLibSymbolUnit( int& aUnit );

    KI_SDK_EXEC_RESULT GetLibSymbolUnitName( const int& aUnit, std::string& aUnitName );

    KI_SDK_EXEC_RESULT GetLibSymbolReference( std::string& aReference );

    KI_SDK_EXEC_RESULT GetLibSymbolValue( std::string& aValue );

    KI_SDK_EXEC_RESULT GetLibSymbolFootprint( std::string& aFootprint );

    KI_SDK_EXEC_RESULT GetLibSymbolDataSheet( std::string& aDataSheet );

    KI_SDK_EXEC_RESULT GetLibSymbolDescription( std::string& aDescription );

    KI_SDK_EXEC_RESULT GetLibSymbolFieldsMap( std::map<std::string, std::string>& aFieldsMap );

    KI_SDK_EXEC_RESULT SetLibSymbolUnitCount( const int& aUnitCount );

    KI_SDK_EXEC_RESULT SetLibSymbolUnit( const int& aUnit );

    // KI_SDK_EXEC_RESULT SetLibSymbolUnitName( const int& aUnit, const std::string& aUnitName );

    KI_SDK_EXEC_RESULT AddLibSymbolField( const std::string& aFieldName, const std::string& aFieldText );

    KI_SDK_EXEC_RESULT ShowLibSymbolFieldByName( const std::string& aFieldName, const bool& isShow , const bool& isShowName = false);

    KI_SDK_EXEC_RESULT SetLibSymbolPower( const bool& aPower);

    KI_SDK_EXEC_RESULT IsLibSymbolPower( bool& aIsPower);

    KI_SDK_EXEC_RESULT GetLibSymbolExcludeFromBoard( bool& aExclude );

    KI_SDK_EXEC_RESULT GetLibSymbolExcludeFromSimulation( bool& aExclude );

    KI_SDK_EXEC_RESULT GetLibSymbolExcludeFromBom( bool& aExclude );

    KI_SDK_EXEC_RESULT SetLibSymbolExcludeFromBoard( const bool& aExclude );

    KI_SDK_EXEC_RESULT SetLibSymbolExcludeFromSimulation( const bool& aExclude );

    KI_SDK_EXEC_RESULT SetLibSymbolExcludeFromBom( const bool& aExclude );

    KI_SDK_EXEC_RESULT GetLibSymbolPinNameOffset( int& mils );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNameOffset( const int& mils );

    KI_SDK_EXEC_RESULT GetLibSymbolPinNumbersIsShow( bool& isShow);

    KI_SDK_EXEC_RESULT GetLibSymbolPinNamesIsShow( bool& isShow );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNumbersShow( const bool& aShow );

    KI_SDK_EXEC_RESULT SetLibSymbolPinNamesShow( const bool& aShow );

    KI_SDK_EXEC_RESULT SetLibSymbolPinInside( const bool& aInside );

    KI_SDK_EXEC_RESULT GetLibSymbolAlternateBodyStyle( bool& aStyle );

    // KI_SDK_EXEC_RESULT SetLibSymbolAlternateBodyStyle( const bool& aStyle );
};


#endif