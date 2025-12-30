#ifndef SCH_PRIMITIVES_CIRCLE_H
#define SCH_PRIMITIVES_CIRCLE_H


#include "sch_primitives_item.h"

class SCH_PRIMITIVES_CIRCLE : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_CIRCLE( SCH_BASE_FRAME* aFrame );


    KI_SDK_EXEC_RESULT      CreateCircle(const VECTOR2I& center, int radius, std::string& aKID);
};

#endif