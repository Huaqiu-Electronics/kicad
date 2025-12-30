#ifndef SCH_PRIMITIVES_SCHEMATIC_TEXT_H
#define SCH_PRIMITIVES_SCHEMATIC_TEXT_H

#include "sch_primitives_item.h"

class SCH_PRIMITIVES_SCHEMATIC_TEXT : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_SCHEMATIC_TEXT( SCH_BASE_FRAME* aFrame );

    KI_SDK_EXEC_RESULT  CreateSchematicText(const VECTOR2I& position, const std::string& text, std::string& aKID);
};



#endif