#ifndef CARD_MODEL_H
#define CARD_MODEL_H

#include "cocos2d.h"

/**
 * @enum CardSuit
 * @brief 卡牌花色枚举
 */
enum CardSuit {
    SUIT_CLUBS,     // 梅花
    SUIT_DIAMONDS,  // 方块
    SUIT_HEARTS,    // 红桃
    SUIT_SPADES     // 黑桃
};

/**
 * @enum CardFace
 * @brief 卡牌点数枚举
 */
enum CardFace {
    FACE_ACE = 1,   // A
    FACE_TWO,
    FACE_THREE,
    FACE_FOUR,
    FACE_FIVE,
    FACE_SIX,
    FACE_SEVEN,
    FACE_EIGHT,
    FACE_NINE,
    FACE_TEN,
    FACE_JACK,      // J
    FACE_QUEEN,     // Q
    FACE_KING       // K
};

/**
 * @class CardModel
 * @brief 卡牌数据模型类
 */
class CardModel {
public:
    CardModel(CardFace face, CardSuit suit, bool isOpen = false);

    CardFace getFace() const { return _face; }
    CardSuit getSuit() const { return _suit; }
    int getFaceValue() const { return (int)_face; }
    cocos2d::Vec2 getPosition() const { return _position; }
    bool isOpen() const { return _isOpen; }
    int getId() const { return _id; }

    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    void setOpen(bool open) { _isOpen = open; }

    std::string getFaceString() const;
    cocos2d::Color3B getColor() const;

private:
    static int _nextId;
    int _id;
    CardFace _face;
    CardSuit _suit;
    cocos2d::Vec2 _position;
    bool _isOpen;
};

#endif