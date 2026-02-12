#ifndef CARD_MODEL_H
#define CARD_MODEL_H

#include "cocos2d.h"

/**
 * @enum CardSuit
 * @brief 卡牌花色枚举
 *
 * 对应四种花色：
 * - SUIT_CLUBS    : 梅花
 * - SUIT_DIAMONDS : 方块
 * - SUIT_HEARTS   : 红桃
 * - SUIT_SPADES   : 黑桃
 */
enum CardSuit { SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES };

/**
 * @enum CardFace
 * @brief 卡牌点数枚举
 *
 * 从1开始对应：
 * - 1  : A
 * - 2~10 : 对应数字
 * - 11 : J
 * - 12 : Q
 * - 13 : K
 */
enum CardFace {
    FACE_ACE = 1, FACE_TWO, FACE_THREE, FACE_FOUR, FACE_FIVE, FACE_SIX,
    FACE_SEVEN, FACE_EIGHT, FACE_NINE, FACE_TEN, FACE_JACK, FACE_QUEEN, FACE_KING
};

/**
 * @class CardModel
 * @brief 卡牌数据模型类
 *
 * 职责：
 * - 存储单张卡牌的静态数据（点数、花色、位置、翻开状态）
 * - 为每张卡牌分配唯一ID
 * - 不包含任何视图逻辑，仅作为数据容器
 *
 * 使用场景：
 * - 被 GameModel 持有，用于表示游戏中的实体卡牌
 * - 被 CardView 关联，用于显示卡牌
 */
class CardModel {
public:
    /**
     * @brief 构造函数
     * @param face  点数（CardFace枚举）
     * @param suit  花色（CardSuit枚举）
     * @param isOpen 初始翻开状态，默认为false
     */
    CardModel(CardFace face, CardSuit suit, bool isOpen = false);

    // ---------- Getter 方法 ----------
    CardFace getFace() const { return _face; }
    CardSuit getSuit() const { return _suit; }
    int getFaceValue() const { return (int)_face; }
    cocos2d::Vec2 getPosition() const { return _position; }
    bool isOpen() const { return _isOpen; }
    int getId() const { return _id; }

    // ---------- Setter 方法 ----------
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    void setOpen(bool open) { _isOpen = open; }

    /**
     * @brief 获取卡牌的点数字符串（A、2~10、J、Q、K）
     */
    std::string getFaceString() const;

    /**
     * @brief 获取卡牌显示颜色（红/黑）
     * @return RED（红桃/方块）或 BLACK（梅花/黑桃）
     */
    cocos2d::Color3B getColor() const;

private:
    static int _nextId;        // 静态计数器，用于生成唯一ID
    int _id;                  // 卡牌唯一标识符
    CardFace _face;          // 点数
    CardSuit _suit;          // 花色
    cocos2d::Vec2 _position; // 当前位置（设计分辨率坐标）
    bool _isOpen;            // 是否翻开（true=正面，false=背面）
};

#endif