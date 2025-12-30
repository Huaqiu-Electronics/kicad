#ifndef SCH_PRIMITIVES_RECTANGLE_H
#define SCH_PRIMITIVES_RECTANGLE_H

#include <string>
#include <math/vector2d.h>
enum class KI_SDK_EXEC_RESULT : int;

#include "sch_primitives_item.h"

class SCH_PRIMITIVES_RECTANGLE : public SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_RECTANGLE( SCH_BASE_FRAME* aFrame );


    KI_SDK_EXEC_RESULT CreateRectangle( const VECTOR2I& pos, int width, int height,  std::string& aKID);
};




#endif