#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "models/CardModel.h"

/**
 * @class CardView
 * @brief 卡牌视图类，继承自cocos2d::Sprite
 */
class CardView : public cocos2d::Sprite {
public:
    enum CardSize { SIZE_BIG };

    static CardView* create(CardModel* model, CardSize size = SIZE_BIG);
    bool initWithModel(CardModel* model, CardSize size);

    void updateView();
    CardModel* getModel() const { return _model; }
    CardSize getCardSize() const { return _size; }
    void setClickCallback(const std::function<void(CardModel*)>& callback);

private:
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    std::string getBigImagePath() const;
    std::string getSmallImagePath() const;
    std::string getSuitIconPath() const;
    std::string getBackImagePath() const;

    CardModel* _model;
    CardSize _size;
    cocos2d::Sprite* _bgSprite;
    cocos2d::Sprite* _bigSprite;
    cocos2d::Sprite* _smallSprite;
    cocos2d::Sprite* _suitSprite;
    cocos2d::Sprite* _backSprite;
    std::function<void(CardModel*)> _clickCallback;
    bool _isClickable;
};

#endif