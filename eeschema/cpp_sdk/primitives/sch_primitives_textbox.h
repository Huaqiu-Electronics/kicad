#ifndef SCH_PRIMITIVES_TEXTBOX_H
#define SCH_PRIMITIVES_TEXTBOX_H

#include "sch_primitives_item.h"

class SCH_PRIMITIVES_TEXTBOX : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_TEXTBOX( SCH_BASE_FRAME* aFrame );

    KI_SDK_EXEC_RESULT  CreateTextBox(const VECTOR2I& position, int width, int height, const std::string& text, std::string& aKID);
};



#endif