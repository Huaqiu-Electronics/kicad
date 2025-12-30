#ifndef SCH_PRIMITIVES_SYMBOL_H
#define SCH_PRIMITIVES_SYMBOL_H

#include <string>
#include <vector>
#include <map>
#include <math/vector2d.h>
#include <math/box2.h>
#include <wx/string.h>
#include <gal/color4d.h>


enum class KI_SDK_EXEC_RESULT : int;
enum class PIN_ORIENTATION;
enum class ELECTRICAL_PINTYPE;
enum class GRAPHIC_PINSHAPE;

class SCH_PRIMITIVES_SYMBOL
{
public:
    KI_SDK_EXEC_RESULT PlaceSymbol( const wxString& aFileName, const wxString& symbol_nick_name,
                                     const VECTOR2I& position, std::string& aKID );

    KI_SDK_EXEC_RESULT GetAllSymbolKIID( std::vector<std::string>& allKIDList );

    KI_SDK_EXEC_RESULT GetSymbolKIIDByReference( const std::string& reference, std::vector<std::string>& aKIDList );

    KI_SDK_EXEC_RESULT GetSymbolReferenceByKIID( const std::string& aKID, std::string& aReference );

    KI_SDK_EXEC_RESULT GetSymbolValueByKIID( const std::string& aKID, std::string& aValue );

    KI_SDK_EXEC_RESULT GetSymbolFpByKIID( const std::string& aKID, std::string& aFootprint );

    KI_SDK_EXEC_RESULT GetSymbolDataSheetByKIID( const std::string& aKID, std::string& aDataSet );

    KI_SDK_EXEC_RESULT GetSymbolDescriptionByKIID( const std::string& aKID, std::string& aDescription );

    KI_SDK_EXEC_RESULT SetSymbolReferenceByKIID( const std::string& aKID, const std::string& aReference );

    KI_SDK_EXEC_RESULT SetSymbolValueByKIID( const std::string& aKID, const std::string& aValue );

    KI_SDK_EXEC_RESULT SetSymbolFpByKIID( const std::string& aKID, const std::string& aFootprint );

    KI_SDK_EXEC_RESULT SetSymbolDataSheetByKIID( const std::string& aKID, const std::string& aDataSheet );

    KI_SDK_EXEC_RESULT SetSymbolDescriptionByKIID( const std::string& aKID, const std::string& aDescription );

    KI_SDK_EXEC_RESULT GetSymbolLibName( const std::string& aKID, std::string& aLibNickName, std::string& aLibItemName,
                                         std::string& aFormatName );

    KI_SDK_EXEC_RESULT GetSymbolFieldsMap( const std::string& aKID, std::map<std::string, std::string>& aMap );


    KI_SDK_EXEC_RESULT AddSymbolField( const std::string& aKID, const std::string& aName, const std::string& aText,
                                       bool aVisible = false );

    KI_SDK_EXEC_RESULT RemoveSymbolField( const std::string& aKID, const std::string& aName );

    KI_SDK_EXEC_RESULT GetSymbolPinCountsInAllUnits( const std::string& aKID, int& aFullNumber );

    KI_SDK_EXEC_RESULT GetSymbolBodyBoundBox( const std::string& aKID, BOX2I& aBox );

    KI_SDK_EXEC_RESULT GetSymbolUnitPinNames( const std::string& aKID, std::vector<std::string>& aNameList );

    KI_SDK_EXEC_RESULT GetSymbolUnitPinNumberList( const std::string& aKID, std::vector<std::string>& aNumberList );

    KI_SDK_EXEC_RESULT GetSymbolUnitPinNameByNumber( const std::string& aKID, const std::string& aNumber,
                                                  std::string& aName );

    KI_SDK_EXEC_RESULT HighlightSymbolPinByNumber( const std::string& aKID, const std::string& aNumber,
                                                   const KIGFX::COLOR4D& aColor = KIGFX::COLOR4D::UNSPECIFIED,
                                                   bool                  aMarker = false );

    KI_SDK_EXEC_RESULT UnHighlightSymbolPinByNumber( const std::string& aKID, const std::string& aNumber, bool aMarker = true);

    KI_SDK_EXEC_RESULT GetSymbolUnitPinPosition( const std::string aKID, const std::string& aNumber,
                                                 VECTOR2I& aPosition );

    KI_SDK_EXEC_RESULT GetSymbolUnitPinOrientation( const std::string& aKID, const std::string& aNumber,
                                                    PIN_ORIENTATION& aOrientation );

    KI_SDK_EXEC_RESULT PlaceLabelOnSymbolPin( const std::string& aKID, const std::string& aNumber,
                                              const std::string& aNetLabel );

    KI_SDK_EXEC_RESULT GetSymbolPinType( const std::string& aKID, const std::string& aNumber,
                                         ELECTRICAL_PINTYPE& aPinType );

    KI_SDK_EXEC_RESULT GetSymbolPinKIID( const std::string& aKID, const std::string& aNumber, std::string& aPinKID );


    KI_SDK_EXEC_RESULT MoveSymbolByKIID( const std::string& aKID, const VECTOR2I& aOffset );

    KI_SDK_EXEC_RESULT RotateSymbolByKIID( const std::string& aKID, bool aCCW );

    KI_SDK_EXEC_RESULT GetAllSymbolLibLibrary( std::vector<std::string>& aLibraryName );


};


#endif