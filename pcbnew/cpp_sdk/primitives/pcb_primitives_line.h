#ifndef PCB_PRIMITIVES_LINE_H
#define PCB_PRIMITIVES_LINE_H


#include "pcb_primitives_item.h"


class PCB_PRIMITIVES_LINE : public PCB_PRIMITIVES_ITEM
{
public:
    explicit PCB_PRIMITIVES_LINE( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateLine( const VECTOR2I& start, const VECTOR2I& end, PCB_LAYER_ID layer, int lineWidth,
                                   bool lock, std::string& aKID );

    KI_SDK_EXEC_RESULT GetAllLinesKIID( std::vector<std::string>& allKIDList );

    KI_SDK_EXEC_RESULT GetLineWidth( const std::string& aKID, int& width );

    KI_SDK_EXEC_RESULT GetLineLayer( const std::string& aKID, PCB_LAYER_ID& layer );

    KI_SDK_EXEC_RESULT GetLineLayerName( const std::string& aKID, std::string& layerName );

    KI_SDK_EXEC_RESULT GetLineStart( const std::string& aKID, VECTOR2I& start );

    KI_SDK_EXEC_RESULT GetLineEnd( const std::string& aKID, VECTOR2I& end );

    KI_SDK_EXEC_RESULT GetLineLockState( const std::string& aKID, bool& lock );

    KI_SDK_EXEC_RESULT SetLineWidth( const std::string& aKID, int width );

    KI_SDK_EXEC_RESULT SetLineLayer( const std::string& aKID, PCB_LAYER_ID layer );

    KI_SDK_EXEC_RESULT SetLineStart( const std::string& aKID, const VECTOR2I& start );

    KI_SDK_EXEC_RESULT SetLineEnd( const std::string& aKID, const VECTOR2I& end );

    KI_SDK_EXEC_RESULT SetLineLockState( const std::string& aKID, bool lock );

    KI_SDK_EXEC_RESULT DeleteAllLines();

    KI_SDK_EXEC_RESULT DeleteLineByKIID( const std::string& aKID );
};







#endif // PCB_PRIMITIVES_LINE_H