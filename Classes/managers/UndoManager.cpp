#include "UndoManager.h"

UndoManager::UndoManager() {}

void UndoManager::pushMoveRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
    int oldTopId, const cocos2d::Vec2& oldTopPos) {
    UndoRecord rec;
    rec.type = UndoRecord::MOVE_CARD;
    rec.cardId = cardId;
    rec.fromPos = from;
    rec.toPos = to;
    rec.oldTopId = oldTopId;
    rec.oldTopPos = oldTopPos;
    _stack.push(rec);
}

void UndoManager::pushStackToTopRecord(int cardId, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
    int stackIndex, int oldTopId, const cocos2d::Vec2& oldTopPos) {
    UndoRecord rec;
    rec.type = UndoRecord::STACK_TO_TOP;
    rec.cardId = cardId;
    rec.fromPos = from;
    rec.toPos = to;
    rec.stackIndex = stackIndex;
    rec.oldTopId = oldTopId;
    rec.oldTopPos = oldTopPos;
    _stack.push(rec);
}

bool UndoManager::canUndo() const { return !_stack.empty(); }

UndoRecord UndoManager::popRecord() {
    auto rec = _stack.top();
    _stack.pop();
    return rec;
}

void UndoManager::clear() { while (!_stack.empty()) _stack.pop(); }