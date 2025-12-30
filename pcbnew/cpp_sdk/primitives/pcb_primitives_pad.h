#ifndef PCB_PRIMITIVES_PAD_H
#define PCB_PRIMITIVES_PAD_H

#include <math/vector2d.h>
#include <string>
#include <vector>

class FOOTPRINT_EDIT_FRAME;
class BOARD;
enum class KI_SDK_EXEC_RESULT : int;
enum class PAD_ATTRIB;
enum PCB_LAYER_ID : int;
enum class PAD_SHAPE : int;
enum class PAD_DRILL_SHAPE;

enum class PADSTACK_MODE : int
{
    NORMAL = 0,
    FRONT_INNER_BACK,
    CUSTOM
};


class PCB_PRIMITIVES_PAD
{
public:
    PCB_PRIMITIVES_PAD();
    //放置焊盘
    KI_SDK_EXEC_RESULT PlacePad( const std::string& aNumber,  const VECTOR2I& aPosition, std::string& aKID );
    KI_SDK_EXEC_RESULT PlacePad_old( std::string& aKID );

    //放置SMD类型焊盘
    KI_SDK_EXEC_RESULT PlaceCustomPad( const PAD_ATTRIB& aPadAttrib, const std::string& aNumber, const VECTOR2I& aPosition,
                                    const PAD_SHAPE& aPadShape, const VECTOR2I& aPadSize,
                                    const PAD_DRILL_SHAPE& aDrillShape, const VECTOR2I& aDrillSize, std::string& aKID );

    KI_SDK_EXEC_RESULT DeletePadByKIID( const std::string& aKID );


    KI_SDK_EXEC_RESULT GetPadKIDByNumber( const std::string& aNumber, std::vector<std::string>& aKIDList );

    //获取当前Footprint Editor中所有的焊盘list
    KI_SDK_EXEC_RESULT GetAllPadsKIID( std::vector<std::string>& allKIDList );

    //Get属性
    //获取PAD位置
    KI_SDK_EXEC_RESULT GetPadPosition( const std::string& aKID, VECTOR2I& position );

    //获取PAD编号
    KI_SDK_EXEC_RESULT GetPadNumber( const std::string& aKID, std::string& number );

    //获取PAD的层信息
    KI_SDK_EXEC_RESULT GetPadLayers( const std::string& aKID, std::vector<PCB_LAYER_ID>& aLayerList );

    //获取Net名称
    KI_SDK_EXEC_RESULT GetPadNetName( const std::string& aKID, std::string& netName );

    //获取Net Code
    KI_SDK_EXEC_RESULT GetPadNetCode( const std::string& aKID, int& netCode );

    //获取PAD锁定状态
    KI_SDK_EXEC_RESULT GetPadIsLocked( const std::string& aKID, bool& isLocked );

    //获取Pad属性
    KI_SDK_EXEC_RESULT GetPadAttribute( const std::string& aKID, PAD_ATTRIB& attribute );

    //获取焊盘到芯片中心的距离
    KI_SDK_EXEC_RESULT GetPadToDieLength( const std::string& aKID, double& distance );

    //获取焊盘的局部间距设置
    KI_SDK_EXEC_RESULT GetPadLocalClearance( const std::string& aKID, double& clearance );

    //获取padstack mode
    KI_SDK_EXEC_RESULT GetPadStackMode( const std::string& aKID, PADSTACK_MODE& mode );

    //获取padstack size
    KI_SDK_EXEC_RESULT GetPadStackSize( const std::string& aKID, const PCB_LAYER_ID& layer, VECTOR2I& size );

    //获取padstack 方向
    KI_SDK_EXEC_RESULT GetPadStackDegree( const std::string& aKID, double& degree );

    //获取padstack drill startLayer
    KI_SDK_EXEC_RESULT GetPadStackDrillStartLayer( const std::string& aKID, PCB_LAYER_ID& startLayer );

    //获取padstack drill endLayer
    KI_SDK_EXEC_RESULT GetPadStackDrillEndLayer( const std::string& aKID, PCB_LAYER_ID& endLayer );

    //获取padstack drill size
    KI_SDK_EXEC_RESULT GetPadStackDrillSize( const std::string& aKID, VECTOR2I& size );

    //设置PAD位置
    KI_SDK_EXEC_RESULT SetPadPosition( const std::string& aKID, const VECTOR2I& position );

    KI_SDK_EXEC_RESULT MovePad( const std::string& aKID, const VECTOR2I& offset );

    KI_SDK_EXEC_RESULT RotatePad( const std::string& aKID, double degree );

    //设置PAD编号
    KI_SDK_EXEC_RESULT SetPadNumber( const std::string& aKID, const std::string& number );

    //设置PAD Net
    KI_SDK_EXEC_RESULT SetPadNetCode( const std::string& aKID, int netCode );

    //设置PAD属性
    KI_SDK_EXEC_RESULT SetPadAttribute( const std::string& aKID, PAD_ATTRIB attribute );

    //设置PAD锁定状态
    KI_SDK_EXEC_RESULT SetPadIsLocked( const std::string& aKID, bool isLocked );

    //设置焊盘到芯片中心的距离
    KI_SDK_EXEC_RESULT SetPadToDieLength( const std::string& aKID, double distance );

    //设置焊盘的局部间距设置
    KI_SDK_EXEC_RESULT SetPadLocalClearance( const std::string& aKID, double clearance );

    //设置padstack mode
    KI_SDK_EXEC_RESULT SetPadStackMode( const std::string& aKID, PADSTACK_MODE mode );

    //设置padstack size
    KI_SDK_EXEC_RESULT SetPadStackSize( const std::string& aKID, VECTOR2I size ); 

    //设置padstack 方向
    KI_SDK_EXEC_RESULT SetPadStackDegree( const std::string& aKID, double degree );

    //设置padstack drill startLayer
    KI_SDK_EXEC_RESULT SetPadStackDrillStartLayer( const std::string& aKID, PCB_LAYER_ID startLayer );

    //设置padstack drill endLayer
    KI_SDK_EXEC_RESULT SetPadStackDrillEndLayer( const std::string& aKID, PCB_LAYER_ID endLayer );

    //设置padstack drill size
    KI_SDK_EXEC_RESULT SetPadStackDrillSize( const std::string& aKID, VECTOR2I size );

    KI_SDK_EXEC_RESULT SetPadDrillShape( const std::string& aKID,const PAD_DRILL_SHAPE& aShape );

private:
    KI_SDK_EXEC_RESULT InitFootprintFrameAndBoard();

private:
    FOOTPRINT_EDIT_FRAME* m_footprintframe;
    BOARD*                m_board;
};


#endif