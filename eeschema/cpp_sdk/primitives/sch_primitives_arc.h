#ifndef SCH_PRIMITIVES_ARC_H
#define SCH_PRIMITIVES_ARC_H

#include "sch_primitives_item.h"

class SCH_PRIMITIVES_ARC : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_ARC( SCH_BASE_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateArc(const VECTOR2I& start, const VECTOR2I& end, std::string& aKID);
    
};






#endif