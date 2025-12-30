#ifndef PCB_PRIMITIVES_ARC_H
#define PCB_PRIMITIVES_ARC_H

#include "pcb_primitives_item.h"

class PCB_EDIT_FRAME;
class FOOTPRINT_EDIT_FRAME;

class PCB_PRIMITIVES_ARC : public PCB_PRIMITIVES_ITEM
{
public:
    explicit PCB_PRIMITIVES_ARC( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT      CreateArc(const VECTOR2I& center, const VECTOR2I& start, const VECTOR2I& end, std::string& aKID);

};



#endif