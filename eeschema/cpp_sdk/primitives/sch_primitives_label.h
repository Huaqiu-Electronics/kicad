#ifndef SCH_PRIMITIVES_LABEL_H
#define SCH_PRIMITIVES_LABEL_H


#include <string>
#include <math/vector2d.h>

enum class KI_SDK_EXEC_RESULT : int;

class SCH_PRIMITIVES_LABEL
{
public:
    KI_SDK_EXEC_RESULT CreateCommonLabel( const VECTOR2I& position, const std::string& text, std::string& aKID );

    KI_SDK_EXEC_RESULT CreateClassLabel( const VECTOR2I& position, const std::string& aNetClass,
                                         const std::string& aComponentClass, std::string& aKID );

    KI_SDK_EXEC_RESULT CreateGlobalLabel( const VECTOR2I& position, const std::string& text, std::string& aKID );

    KI_SDK_EXEC_RESULT CreateHierLabel( const VECTOR2I& position, const std::string& text, std::string& aKID );

    KI_SDK_EXEC_RESULT RotateLabel( const std::string& aKID, int angle );

    KI_SDK_EXEC_RESULT MoveLabel( const std::string& aKID, const VECTOR2I& aOffset );
};


#endif