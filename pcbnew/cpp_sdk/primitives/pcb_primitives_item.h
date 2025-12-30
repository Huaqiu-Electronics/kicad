#ifndef PCB_PRIMITIVES_ITEM_H
#define PCB_PRIMITIVES_ITEM_H

#include <string>
#include <vector>
#include <math/vector2d.h>
#include <frame_type.h>


class PCB_EDIT_FRAME;
class FOOTPRINT_EDIT_FRAME;
class BOARD;
class PCB_BASE_EDIT_FRAME;

enum class KI_SDK_EXEC_RESULT : int;


class PCB_PRIMITIVES_ITEM
{
public:
    explicit PCB_PRIMITIVES_ITEM( PCB_BASE_EDIT_FRAME* aFrame );

    virtual ~PCB_PRIMITIVES_ITEM();

    PCB_BASE_EDIT_FRAME* GetBaseEditFrame() const { return m_base_frame; }

    BOARD* GetBoard() const { return m_board; }

    FRAME_T GetFrameType() const;


private:
    PCB_BASE_EDIT_FRAME* m_base_frame;
    BOARD* m_board;
};


#endif