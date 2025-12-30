#ifndef PCB_PRIMITIVES_BEZIER_H
#define PCB_PRIMITIVES_BEZIER_H

#include "pcb_primitives_item.h"

class PCB_BASE_EDIT_FRAME;


class PCB_PRIMITIVES_BEZIER : public PCB_PRIMITIVES_ITEM
{
public:
    explicit PCB_PRIMITIVES_BEZIER( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateBezier( const VECTOR2I& start, const VECTOR2I& c1, const VECTOR2I& c2, const VECTOR2I& end, std::string& aKID);
};



#endif
