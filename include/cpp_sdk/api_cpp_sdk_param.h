#ifndef API_CPP_SDK_PARAM_H
#define API_CPP_SDK_PARAM_H

#include <nlohmann/json.hpp>

enum class API_SYMBOL_CATEGORY : int
{
    // ===================== 基础无源元件 =====================
    RESISTOR,           // 电阻器
    POTENTIOMETER,      // 电位器（可调电阻）
    CAPACITOR,          // 电容器（无极性）
    INDUCTOR,           // 电感器
    CRYSTAL_OSCILLATOR, // 晶体振荡器
    TRANSFORMER,        // 变压器
    FUSE,               // 熔断器
    FERRITE_BEAD,       // 铁氧体磁珠

    // ===================== 半导体元件 =====================
    DIODE,                     // 二极管
    ZENER_DIODE,               // 齐纳/稳压二极管
    SCHOTTKY_DIODE,            // 肖特基二极管
    VARACTOR_DIODE,            // 变容二极管
    LED,                       // 发光二极管
    PHOTODIODE,                // 光电二极管
    TRANSISTOR_NPN,            // NPN型三极管
    TRANSISTOR_PNP,            // PNP型三极管
    MOSFET_N_CHANNEL,          // N沟道MOS场效应管
    MOSFET_P_CHANNEL,          // P沟道MOS场效应管
    JFET_N_CHANNEL,            // N沟道结型场效应管
    JFET_P_CHANNEL,            // P沟道结型场效应管
    IGBT,                      // 绝缘栅双极晶体管
    THYRISTOR,                 // 晶闸管
    TRIAC,                     // 双向可控硅

    // ===================== 集成电路与逻辑器件 =====================
    OPERATIONAL_AMPLIFIER, // 运算放大器
    COMPARATOR,            // 比较器
    LOGIC_GATE_AND,        // 与门
    LOGIC_GATE_NAND,       // 与非门
    LOGIC_GATE_OR,         // 或门
    LOGIC_GATE_NOR,        // 或非门
    LOGIC_GATE_NOT,        // 非门/反相器
    LOGIC_GATE_XOR,        // 异或门
    FLIP_FLOP_D_TYPE,      // D型触发器
    FLIP_FLOP_J_K,         // JK型触发器
    COUNTER,               // 计数器
    DECODER,               // 译码器
    MULTIPLEXER,           // 多路复用器
    MCU,                   // 微控制器
    ADC,                   // 模数转换器
    DAC,                   // 数模转换器

    // ===================== 电源与接地 =====================
    POWER_AC,                    // 交流电源
    POWER_DC,                    // 直流电源
    GROUND,                      // 接地
    GROUND_DIGITAL,              // 数字地
    POWER_PLANE_VCC,             // VCC电源层
    POWER_PLANE_VDD,             // VDD电源层
    BRIDGE_RECTIFIER,            // 全波桥式整流器
    VOLTAGE_REGULATOR_LINEAR,    // 线性电压调节器
    VOLTAGE_REGULATOR_SWITCHING, // 开关型电压调节器
    VOLTAGE_REFERENCE,           // 基准电压源

    // ===================== 接口与机电元件 =====================
    CONNECTOR_GENERIC,  // 通用连接器
    CONNECTOR_USB,      // USB接口
    CONNECTOR_ETHERNET, // 以太网接口
    RELAY,              // 继电器
    SWITCH_SPST,        // 单刀单掷开关
    SWITCH_DPDT,        // 双刀双掷开关
    BUTTON,             // 按钮
    BUZZER,             // 蜂鸣器
    SPEAKER,            // 扬声器
    DISPLAY_7_SEGMENT,  // 7段数码管
    DISPLAY_BAR_GRAPH,  // 条形LED显示器

    // ===================== 特殊功能元件 =====================
    SENSOR_TEMPERATURE, // 温度传感器
    SENSOR_HUMIDITY,    // 湿度传感器
    SENSOR_CURRENT,     // 电流传感器
    SENSOR_PRESSURE,    // 压力传感器
    ISOLATOR_ANALOG,    // 模拟隔离器
    ISOLATOR_DIGITAL,   // 数字隔离器
    MEMORY_EEPROM,      // EEPROM存储器
    MEMORY_FLASH,       // FLASH存储器
    MEMORY_RAM,         // RAM存储器
    MOTOR,              // 电机
    DRIVER_MOTOR,       // 电机驱动器
    DRIVER_LED,         // LED驱动器
    DRIVER_RELAY,       // 继电器驱动器

    // ===================== 预留扩展 =====================
    UNKNOWN // 未知器件类别
};


struct CPP_SDK_API_PARAM
{
    std::string    api;
    nlohmann::json parameter;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( CPP_SDK_API_PARAM, api, parameter )
};


struct API_POINT
{
    double x;
    double y;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_POINT, x, y )
};

struct API_LINE
{
    API_POINT start;
    API_POINT end;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LINE, start, end )
};

struct API_PCB_LAYER_NAME
{
    std::string pcb_layer_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_PCB_LAYER_NAME, pcb_layer_name)
};

struct API_PCB_TRACK
{
    API_POINT start;
    API_POINT end;
    API_PCB_LAYER_NAME layer_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PCB_TRACK, start, end , layer_name)
};

struct API_MULTI_LINES
{
    std::vector<API_LINE>  lines;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_MULTI_LINES, lines)
};

struct API_CIRCLE
{
    API_POINT center;
    double    radius;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CIRCLE, center, radius )
};

struct API_ARC
{
    API_POINT start;
    API_POINT end;
    API_POINT center;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_ARC, start, end ,center)
};

struct API_BEZIER
{
    API_POINT start;
    API_POINT c1;
    API_POINT c2;
    API_POINT end;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_BEZIER, start, c1, c2, end )
};

struct API_RECTANGLE
{
    API_POINT top_left;
    double width;
    double height;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_RECTANGLE, top_left, width, height)
};

struct API_TEXTBOX
{
    API_RECTANGLE box;
    std::string text;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_TEXTBOX, box, text)
};


struct API_TABLE
{
    int rows;
    int cols;
    API_POINT pos;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_TABLE, rows, cols, pos)
};

struct API_HIER_SHEET
{
    API_RECTANGLE box;
    std::string   title;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_HIER_SHEET, box, title)
};

struct API_LABEL
{
    API_POINT position;
    std::string text;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LABEL , position, text)
};

struct API_CLASS_LABEL
{
    API_POINT position;
    std::string net_class;
    std::string component_class;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CLASS_LABEL, position, net_class ,component_class)
};

struct API_SYMBOL_PIN_LABEL
{
    std::string reference;
    std::string pin_number;
    std::string label_text;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_SYMBOL_PIN_LABEL, reference, pin_number, label_text)
};

struct API_QUERY_SYMBOL
{
    std::string reference;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_QUERY_SYMBOL, reference)
};

struct API_PIN_INFO
{
    std::string pin_number;
    std::string pin_name;
    API_POINT pin_position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PIN_INFO, pin_number,pin_name, pin_position)
};

struct API_SYMBOL_PINS_INFO
{
    std::string reference;
    std::string symbol_name;
    std::vector<API_PIN_INFO> pins_info;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_PINS_INFO, reference, symbol_name, pins_info )
};

// Symbol API Parameters
struct API_PLACE_SYMBOL_old
{
    std::string file_name;
    std::string symbol_nick_name;
    API_POINT position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PLACE_SYMBOL_old, file_name, symbol_nick_name, position )
};

struct API_PLACE_SYMBOL
{
    std::string category;
    std::string value;
    std::string reference;
    API_POINT   position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PLACE_SYMBOL, category, value, reference, position )
};

struct API_SYMBOL_LIBRARY
{
    std::string symbol_library_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_LIBRARY, symbol_library_name )
};

struct API_SYMBOL_LIBRARY_LIST
{
    std::vector<API_SYMBOL_LIBRARY> library_list;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_LIBRARY_LIST, library_list )
};

struct API_PCB_LAYER_NAME_LIST
{
    std::vector<API_PCB_LAYER_NAME> pcb_layer_name_list;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_PCB_LAYER_NAME_LIST, pcb_layer_name_list)
};

struct API_SYMBOL_KIID
{
    std::string kiid;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_KIID, kiid )
};

struct API_SYMBOL_REFERENCE
{
    std::string old_reference;
    std::string new_reference;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_REFERENCE, old_reference, new_reference )
};

struct API_SYMBOL_VALUE
{
    std::string reference;
    std::string value;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_VALUE, reference, value )
};

struct API_PCB_REFERENCE_PARAMS
{
    std::string reference;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_PCB_REFERENCE_PARAMS, reference)
};
struct API_PCB_REFERENCE_LIST
{
    std::vector<API_PCB_REFERENCE_PARAMS> reference_list;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PCB_REFERENCE_LIST, reference_list )
};

struct API_PCB_FOOTPRINT_PARAMS
{
    std::string reference;
    std::string fpid;
    API_POINT   position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PCB_FOOTPRINT_PARAMS, reference, fpid, position )
};

struct API_PCB_FOOTPRINT_PARAMS_LIST
{
    std::vector<API_PCB_FOOTPRINT_PARAMS> footprint_list;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PCB_FOOTPRINT_PARAMS_LIST, footprint_list )
};

struct API_SYMBOL_DATASHEET
{
    std::string reference;
    std::string datasheet;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_DATASHEET, reference, datasheet )
};

struct API_SYMBOL_DESCRIPTION
{
    std::string kiid;
    std::string description;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_DESCRIPTION, kiid, description )
};

struct API_SYMBOL_FIELD
{
    std::string kiid;
    std::string name;
    std::string text;
    bool visible;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_FIELD, kiid, name, text, visible )
};

struct API_SYMBOL_PIN_INFO
{
    std::string kiid;
    std::string pin_number;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_PIN_INFO, kiid, pin_number )
};

struct API_SYMBOL_PIN_HIGHLIGHT
{
    std::string kiid;
    std::string pin_number;
    std::string color;
    bool marker;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SYMBOL_PIN_HIGHLIGHT, kiid, pin_number, color, marker )
};

// Line API Parameters
struct API_CREATE_BUS_WIRE_ENTRY
{
    API_POINT position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CREATE_BUS_WIRE_ENTRY, position )
};

struct API_CREATE_JUNCTION
{
    API_POINT position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CREATE_JUNCTION, position )
};

struct API_GET_ALL_LINES_KIID
{
    int layer;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_GET_ALL_LINES_KIID, layer )
};

struct API_LINE_KIID
{
    std::string kiid;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LINE_KIID, kiid )
};

struct API_LINE_WIDTH
{
    std::string kiid;
    int width;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LINE_WIDTH, kiid, width )
};

struct API_LINE_COLOR
{
    std::string kiid;
    int r;
    int g;
    int b;
    int a;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LINE_COLOR, kiid, r, g, b, a )
};

struct API_MOVE_LINE
{
    std::string kiid;
    API_POINT offset;
    int type;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_MOVE_LINE, kiid, offset, type )
};

struct API_SET_LINE_START
{
    std::string kiid;
    API_POINT start;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_LINE_START, kiid, start )
};

struct API_SET_LINE_END
{
    std::string kiid;
    API_POINT end;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_LINE_END, kiid, end )
};



struct API_MOVE_SYMBOL
{
    std::string reference;
    std::string unit;
    API_POINT offset;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_MOVE_SYMBOL, reference,unit, offset )
};

struct API_ROTATE_SYMBOL
{
    std::string reference;
    std::string unit;
    bool ccw;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_ROTATE_SYMBOL, reference,unit, ccw )
};

struct API_MODIFY_SYMBOL_FIELD_VALUE
{
    std::string reference;
    std::string field_name;
    std::string filed_value;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_MODIFY_SYMBOL_FIELD_VALUE, reference, field_name, filed_value)
};


// Sheet API Parameters
struct API_CREATE_SHEET_PIN
{
    std::string pin_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CREATE_SHEET_PIN, pin_name )
};

struct API_GET_SHEET_PAGE_NUMBER
{
    std::string path;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_GET_SHEET_PAGE_NUMBER, path )
};

struct API_GET_SHEET_BY_PAGE_NUMBER
{
    std::string page_number;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_GET_SHEET_BY_PAGE_NUMBER, page_number )
};

// Marker API Parameters
struct API_ADD_MARKER
{
    API_POINT position;
    std::string pin_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_ADD_MARKER, position, pin_id )
};

struct API_CLEAR_MARKER
{
    API_POINT position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_CLEAR_MARKER, position )
};

struct API_FIND_MARKER_BY_POSITION
{
    API_POINT position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_FIND_MARKER_BY_POSITION, position )
};


struct API_CREATE_SYMBOL_LIBRARY
{
    std::string symbol_library_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_CREATE_SYMBOL_LIBRARY, symbol_library_name)
};

struct API_LIB_SYMBOL_PIN
{
    std::string pin_name;
    std::string pin_number;
    API_POINT   position;
    // std::string  orientation;
    // std::string  electrical;
    // std::string  shape;
    // NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LIB_SYMBOL_PIN, pin_name, pin_number, position, orientation, electrical, shape )
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_LIB_SYMBOL_PIN, pin_name, pin_number, position )
};

struct API_PCB_VIA
{
    API_POINT position;
    std::string start_layer;
    std::string end_layer;
    std::string via_type;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_PCB_VIA,position,start_layer,end_layer,via_type)
};

struct API_PCB_PAD
{
    API_POINT   position;
    std::string number;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_PCB_PAD , position , number)
};

struct API_MOVE_PCB_PAD
{
    std::string number;
    API_POINT offset;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_MOVE_PCB_PAD, number, offset )
};

struct API_ROTATE_PCB_PAD
{
    std::string number;
    double         degree;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_ROTATE_PCB_PAD, number, degree )
};

struct API_SET_PAD_NUMBER
{
    std::string old_number;
    std::string new_number;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_PAD_NUMBER, old_number, new_number)
};

struct API_SET_PAD_SIZE
{
    std::string number;
    API_POINT   size;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_PAD_SIZE ,number,size )
};

struct API_SET_PAD_POSITION
{
    std::string number;
    API_POINT   new_position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_PAD_POSITION, number, new_position )
};

struct API_SET_PAD_DRILL_SHAPE
{
    std::string number;
    std::string shape;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_SET_PAD_DRILL_SHAPE, number, shape )
};


struct API_MOVE_PCB_FOOTPRINT
{
    std::string reference;
    API_POINT   offset;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_MOVE_PCB_FOOTPRINT , reference, offset)
};

struct API_SET_FOOTPRINT_POSITION
{
    std::string reference;
    API_POINT   new_position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_SET_FOOTPRINT_POSITION, reference, new_position)
};

struct API_ROTATE_FOOTPRINT
{
    std::string reference;
    double  degree;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(API_ROTATE_FOOTPRINT, reference, degree)
};

struct API_FRAME_PARAMS
{
    // std::string cmd;
    std::string frame_type;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE( API_FRAME_PARAMS, frame_type)
};

#endif