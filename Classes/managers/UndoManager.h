#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "cocos2d.h"
#include <stack>

/**
 * @struct UndoRecord
 * @brief 回退记录结构体
 */
struct UndoRecord {
    enum Type { MOVE_CARD, STACK_TO_TOP };
    Type type;

    int cardId;
    cocos2d::Vec2 fromPos;
    cocos2d::Vec2 toPos;

    // MOVE_CARD 专用
    int oldTopId;
    cocos2d::Vec2 oldTopPos;

    // STACK_TO_TOP 专用
    int stackIndex;
};

/**
 * @class UndoManager
 * @brief 回退管理器
 */
class UndoManager {
public:
    UndoManager();

    void pushMoveRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        int oldTopId, const cocos2d::Vec2& oldTopPos);
    void pushStackToTopRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        int stackIndex, int oldTopId, const cocos2d::Vec2& oldTopPos);

    bool canUndo() const;
    UndoRecord popRecord();
    void clear();

private:
    std::stack<UndoRecord> _stack;
};

#endif