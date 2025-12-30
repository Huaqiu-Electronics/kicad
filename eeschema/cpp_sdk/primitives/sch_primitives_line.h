#ifndef SCH_PRIMITIVES_LINE_H
#define SCH_PRIMITIVES_LINE_H

enum SCH_LAYER_ID : int;

#include "sch_primitives_item.h"


namespace KIGFX
{
class COLOR4D;
}


class SCH_PRIMITIVES_LINE : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_LINE( SCH_BASE_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateLine( const VECTOR2I& start, const VECTOR2I& end, SCH_LAYER_ID layer, int lineWidth,
                                   bool lock, std::string& aKID );
    KI_SDK_EXEC_RESULT CreateBusWireEntry( const VECTOR2I& position, std::string& aKID );

    KI_SDK_EXEC_RESULT CreateJunction( const VECTOR2I& position, std::string& aKID );

    KI_SDK_EXEC_RESULT GetAllLinesKIID( const SCH_LAYER_ID& layer, std::vector<std::string>& allKIDList );

    KI_SDK_EXEC_RESULT GetLineWidth( const std::string& aKID, double& aLineWidth );

    KI_SDK_EXEC_RESULT GetWireNetName( const std::string& aKID, std::string& aNetName );

    KI_SDK_EXEC_RESULT GetWireDriver( const std::string& aKID, std::string& aDriver );

    KI_SDK_EXEC_RESULT SetLineColor( const std::string& aKID, const KIGFX::COLOR4D& aColor );

    KI_SDK_EXEC_RESULT SetLineWidth( const std::string& aKID, const int& aLineWidth );

    KI_SDK_EXEC_RESULT MoveLine( const std::string& aKID, const VECTOR2I& aOffset, const int& type );

    KI_SDK_EXEC_RESULT SetLineStart( const std::string& aKID, const VECTOR2I& aStart );

    KI_SDK_EXEC_RESULT SetLineEnd( const std::string& aKID, const VECTOR2I& aEnd );

private:
    KI_SDK_EXEC_RESULT createLineInSymbolFrame( const VECTOR2I& start, const VECTOR2I& end, SCH_LAYER_ID layer,
                                                int lineWidth, bool lock, std::string& aKID );
    KI_SDK_EXEC_RESULT createLineInSchEditFrame( const VECTOR2I& start, const VECTOR2I& end, SCH_LAYER_ID layer,
                                                 int lineWidth, bool lock, std::string& aKID );
};


#endif