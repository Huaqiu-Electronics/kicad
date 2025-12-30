#ifndef PCB_PRIMITIVES_TEXTBOX_H
#define PCB_PRIMITIVES_TEXTBOX_H

#include "pcb_primitives_item.h"


class PCB_PRIMITIVES_TEXTBOX : public PCB_PRIMITIVES_ITEM
{

public:

    explicit PCB_PRIMITIVES_TEXTBOX( PCB_BASE_EDIT_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateTextBox( const VECTOR2I& position, int width, int height, const std::string& text , std::string& aKID);

    KI_SDK_EXEC_RESULT CreateText( const VECTOR2I& position, const std::string& text, std::string& aKID );
};



#endif