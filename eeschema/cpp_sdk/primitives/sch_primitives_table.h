#ifndef SCH_PRIMITIVES_TABLE_H
#define SCH_PRIMITIVES_TABLE_H


#include <string>
#include <math/vector2d.h>

enum class KI_SDK_EXEC_RESULT : int;

class SCH_PRIMITIVES_TABLE
{
public:
    KI_SDK_EXEC_RESULT  CreateTable(const VECTOR2I& position, int rows, int cols, std::string& aKID);
};








#endif