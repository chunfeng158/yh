#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/CardView.h"
#include "managers/UndoManager.h"
#include <unordered_map>

/**
 * @class GameController
 * @brief 游戏主控制器
 */
class GameController : public cocos2d::Ref {
public:
    static GameController* create();
    bool init();

    void attach(cocos2d::Node* scene);
    void setGameModel(GameModel* model);
    void setStackBasePos(const cocos2d::Vec2& pos);
    void setTopCardFixedPos(const cocos2d::Vec2& pos);
    void undo();

private:
    GameController();
    ~GameController();

    void onCardClicked(CardModel* card);
    void performMatch(CardModel* playFieldCard);
    void performStackToTop(CardModel* stackCard);
    void refreshAllViews();
    void refreshStackViews();
    CardView* createCardView(CardModel* model);

    void undoMoveCard(const UndoRecord& rec);
    void undoStackToTop(const UndoRecord& rec);

    cocos2d::Node* _scene;
    GameModel* _model;
    UndoManager* _undoManager;
    std::unordered_map<int, CardView*> _cardViewMap;
    bool _isAnimating;
    const cocos2d::Vec2 STACK_OFFSET = cocos2d::Vec2(15, -15);
};

#endif