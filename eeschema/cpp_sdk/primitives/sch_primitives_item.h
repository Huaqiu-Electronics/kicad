#ifndef SCH_PRIMITIVES_ITEM_H
#define SCH_PRIMITIVES_ITEM_H

#include <string>
#include <vector>
#include <math/vector2d.h>
#include <frame_type.h>

enum class KI_SDK_EXEC_RESULT : int;

class SCH_BASE_FRAME;
class EDA_ITEM;
class SCH_ITEM;
enum SCH_LAYER_ID : int;

class SCH_PRIMITIVES_ITEM
{
public:
    explicit SCH_PRIMITIVES_ITEM( SCH_BASE_FRAME* aFrame );

    virtual ~SCH_PRIMITIVES_ITEM();

public:
    SCH_BASE_FRAME* GetBaseFrame() const { return m_frame; }

    EDA_ITEM* GetItemParent() const { return m_parent; }


    void AddItemToScreen( SCH_ITEM* item );

    SCH_LAYER_ID GetLayerId() const { return m_layer_id; }

    void SetLayerId( const SCH_LAYER_ID& aLayerId ) { m_layer_id = aLayerId; }

    FRAME_T GetFrameType() const;

private:
    void initItemParent();

private:
    SCH_BASE_FRAME* m_frame;
    EDA_ITEM*       m_parent;

    SCH_LAYER_ID    m_layer_id;
};


#endif