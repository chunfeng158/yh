#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "cocos2d.h"
#include <stack>

/**
 * @struct UndoRecord
 * @brief 回退记录结构体，存储一次操作的所有必要信息
 *
 * 包含两种类型：
 * - MOVE_CARD    : 主牌区匹配移动
 * - STACK_TO_TOP : 堆牌区→顶牌无条件移动
 */
struct UndoRecord {
    enum Type { MOVE_CARD, STACK_TO_TOP };
    Type type;                 // 操作类型

    int cardId;                // 被操作的卡牌ID
    cocos2d::Vec2 fromPos;    // 起始位置
    cocos2d::Vec2 toPos;      // 目标位置

    // MOVE_CARD 专用
    int oldTopId;             // 旧顶牌ID
    cocos2d::Vec2 oldTopPos; // 旧顶牌位置

    // STACK_TO_TOP 专用
    int stackIndex;           // 被移动的堆牌在列表中的原索引
};

/**
 * @class UndoManager
 * @brief 回退管理器，负责维护回退栈并提供记录/撤销接口
 *
 * 职责：
 * - 存储UndoRecord栈
 * - 提供记录操作的方法
 * - 不包含任何业务逻辑，仅作为数据容器
 *
 * 使用场景：
 * - 作为GameController的成员变量
 * - 在执行游戏动作时调用push*Record()
 * - 在撤销时popRecord()并返回给控制器处理
 */
class UndoManager {
public:
    UndoManager();

    /**
     * @brief 记录一次匹配移动操作
     * @param cardId 被移动的卡牌ID
     * @param from   原位置
     * @param to     目标位置（顶牌固定位置）
     * @param oldTopId 旧顶牌ID
     * @param oldTopPos 旧顶牌位置
     */
    void pushMoveRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        int oldTopId, const cocos2d::Vec2& oldTopPos);

    /**
     * @brief 记录一次堆牌区→顶牌移动操作
     * @param cardId 被移动的堆牌ID
     * @param from   原偏移位置
     * @param to     目标位置（顶牌固定位置）
     * @param stackIndex 在堆牌列表中的原索引
     * @param oldTopId 旧顶牌ID
     * @param oldTopPos 旧顶牌位置
     */
    void pushStackToTopRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        int stackIndex, int oldTopId, const cocos2d::Vec2& oldTopPos);

    bool canUndo() const;

    /**
     * @brief 弹出栈顶记录（不移除引用）
     * @return UndoRecord 栈顶记录的副本
     */
    UndoRecord popRecord();

    void clear();

private:
    std::stack<UndoRecord> _stack; // 回退记录栈
};

#endif