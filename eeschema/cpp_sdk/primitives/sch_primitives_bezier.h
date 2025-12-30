#ifndef SCH_PRIMITIVES_BEZIER_H
#define SCH_PRIMITIVES_BEZIER_H


#include "sch_primitives_item.h"

class SCH_PRIMITIVES_BEZIER : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_BEZIER( SCH_BASE_FRAME* aFrame );

    KI_SDK_EXEC_RESULT CreateBezier(const VECTOR2I& start, const VECTOR2I& c1, const VECTOR2I& c2, const VECTOR2I& end, std::string& aKID);
    
};


#endif