#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>

/**
 * @class GameModel
 * @brief 游戏全局状态数据模型
 */
class GameModel {
public:
    GameModel();
    ~GameModel();

    // ---------- 主牌区 ----------
    void addPlayFieldCard(CardModel* card);
    void removePlayFieldCard(int index);
    void removePlayFieldCard(CardModel* card);
    CardModel* getPlayFieldCard(int index) const;
    const std::vector<CardModel*>& getAllPlayFieldCards() const { return _playFieldCards; }
    int getPlayFieldCardCount() const { return (int)_playFieldCards.size(); }
    int getPlayFieldCardIndex(CardModel* card) const;

    // ---------- 堆牌区 ----------
    void addStackCard(CardModel* card);
    void removeStackCard(int index);
    void removeStackCard(CardModel* card);
    CardModel* getStackCard(int index) const;
    CardModel* getTopStackCard() const;
    const std::vector<CardModel*>& getAllStackCards() const { return _stackCards; }
    int getStackCardCount() const { return (int)_stackCards.size(); }
    int getStackCardIndex(CardModel* card) const;
    void setStackBasePos(const cocos2d::Vec2& pos) { _stackBasePos = pos; }
    cocos2d::Vec2 getStackBasePos() const { return _stackBasePos; }

    // ---------- 顶牌区 ----------
    void setTopCard(CardModel* card);
    CardModel* getTopCard() const { return _topCard; }
    void setTopCardFixedPos(const cocos2d::Vec2& pos) { _topCardFixedPos = pos; }
    cocos2d::Vec2 getTopCardFixedPos() const { return _topCardFixedPos; }

    void clear();

private:
    std::vector<CardModel*> _playFieldCards;
    std::vector<CardModel*> _stackCards;
    CardModel* _topCard;
    cocos2d::Vec2 _stackBasePos;
    cocos2d::Vec2 _topCardFixedPos;
};

#endif