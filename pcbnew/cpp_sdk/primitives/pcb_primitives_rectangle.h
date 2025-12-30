#ifndef PCB_PRIMITIVES_RECTANGLE_H
#define PCB_PRIMITIVES_RECTANGLE_H

#include "pcb_primitives_item.h"

class PCB_BASE_EDIT_FRAME;


class PCB_PRIMITIVES_RECTANGLE : public PCB_PRIMITIVES_ITEM
{
public:

    explicit PCB_PRIMITIVES_RECTANGLE( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateRectangle( const VECTOR2I& position, int width, int height , std::string& aKID);

};





#endif