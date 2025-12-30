#ifndef PCB_PRIMITIVES_TABLE_H
#define PCB_PRIMITIVES_TABLE_H

#include "pcb_primitives_item.h"

class PCB_PRIMITIVES_TABLE : public PCB_PRIMITIVES_ITEM
{
public:

    explicit PCB_PRIMITIVES_TABLE( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateTable( const VECTOR2I& position, const VECTOR2I& gridSize, int rows, int cols , std::string& aKID);

};








#endif