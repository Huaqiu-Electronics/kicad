#ifndef PCB_PRIMITIVES_VIA_H
#define PCB_PRIMITIVES_VIA_H

#include <math/vector2d.h>
#include <string>
#include <vector>

enum class VIATYPE : int;
enum PCB_LAYER_ID: int;

enum class KI_SDK_EXEC_RESULT : int;

class PCB_PRIMITIVES_VIA
{
    public:

        KI_SDK_EXEC_RESULT          CreateVia(int netCode, const VECTOR2I& position, VIATYPE& aType,
                                             PCB_LAYER_ID& first_layer, PCB_LAYER_ID& last_layer, int drill, int width, bool lock, std::string& aKID);

        //获取当所有的VIA
        KI_SDK_EXEC_RESULT          GetAllViaKIID(std::vector<std::string>& allKID);

        //Get属性
        //获取VIA类型
        KI_SDK_EXEC_RESULT          GetViaType(const std::string& aKID, VIATYPE& viaType);    

        //获取VIA位置
        KI_SDK_EXEC_RESULT          GetViaPosition(const std::string& aKID, VECTOR2I& position);

        //获取VIA参数drill
        KI_SDK_EXEC_RESULT          GetViaDrill(const std::string& aKID, int& drill);

        //获取VIA参数width
        KI_SDK_EXEC_RESULT          GetViaWidth(const std::string& aKID ,const PCB_LAYER_ID& aLayer, int& width);

        //获取VIA是否lock
        KI_SDK_EXEC_RESULT          GetViaLockState(const std::string& aKID, bool& lock);

        //Set属性
        //设置VIA位置
        KI_SDK_EXEC_RESULT          SetViaPosition(const std::string& aKID, const VECTOR2I& position);

        //设置VIA类型
        KI_SDK_EXEC_RESULT          SetViaType(const std::string& aKID, const VIATYPE& viaType);

        //设置VIA参数drill
        KI_SDK_EXEC_RESULT          SetViaDrill(const std::string& aKID, int drill);

        //设置VIA参数lock状态
        KI_SDK_EXEC_RESULT          SetViaLocked(const std::string& aKID, bool lock);

        //删除
        KI_SDK_EXEC_RESULT          DeleteViaByKIID(const std::string& aKID);
        
};



#endif