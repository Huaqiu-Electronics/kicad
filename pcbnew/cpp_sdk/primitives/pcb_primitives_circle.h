#ifndef PCB_PRIMITIVES_CIRCLE_H
#define PCB_PRIMITIVES_CIRCLE_H

#include "pcb_primitives_item.h"

class PCB_BASE_EDIT_FRAME;

class PCB_PRIMITIVES_CIRCLE : public PCB_PRIMITIVES_ITEM
{
public:
    explicit PCB_PRIMITIVES_CIRCLE( PCB_BASE_EDIT_FRAME* aFrame );
    
    KI_SDK_EXEC_RESULT CreateCircle( const VECTOR2I& center, int radius , std::string& aKID);
};






#endif