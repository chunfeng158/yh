#include "CardView.h"

USING_NS_CC;

CardView* CardView::create(CardModel* model, CardSize size) {
    auto view = new CardView();
    if (view && view->initWithModel(model, size)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::initWithModel(CardModel* model, CardSize size) {
    if (!Sprite::init()) return false;

    _model = model;
    _size = size;
    _clickCallback = nullptr;
    _isClickable = true;

    const float W = 140.0f, H = 196.0f;
    setContentSize(Size(W, H));

    // 正面背景（card_back.png作为纹理）
    _bgSprite = Sprite::create(getBackImagePath());
    if (_bgSprite) {
        float sx = W / _bgSprite->getContentSize().width;
        float sy = H / _bgSprite->getContentSize().height;
        _bgSprite->setScale(sx, sy);
        _bgSprite->setPosition(W / 2, H / 2);
        _bgSprite->setLocalZOrder(0);
        addChild(_bgSprite);
    }

    // 背面（与正面相同纹理，但分开控制）
    _backSprite = Sprite::create(getBackImagePath());
    if (_backSprite) {
        float sx = W / _backSprite->getContentSize().width;
        float sy = H / _backSprite->getContentSize().height;
        _backSprite->setScale(sx, sy);
        _backSprite->setPosition(W / 2, H / 2);
        _backSprite->setLocalZOrder(0);
        addChild(_backSprite);
    }

    // 中央大图
    auto bigTex = Director::getInstance()->getTextureCache()->addImage(getBigImagePath());
    if (bigTex) {
        _bigSprite = Sprite::createWithTexture(bigTex);
        float scale = (W * 0.6f) / _bigSprite->getContentSize().width;
        _bigSprite->setScale(scale);
        _bigSprite->setPosition(W / 2, H / 2);
        _bigSprite->setLocalZOrder(2);
        addChild(_bigSprite);
    }

    // 左上角小图
    auto smallTex = Director::getInstance()->getTextureCache()->addImage(getSmallImagePath());
    if (smallTex) {
        _smallSprite = Sprite::createWithTexture(smallTex);
        float scale = (W * 0.25f) / _smallSprite->getContentSize().width;
        _smallSprite->setScale(scale);
        _smallSprite->setAnchorPoint(Vec2(0, 1));
        _smallSprite->setPosition(5, H - 5);
        _smallSprite->setLocalZOrder(2);
        addChild(_smallSprite);
    }

    // 右上角花色图标
    auto suitTex = Director::getInstance()->getTextureCache()->addImage(getSuitIconPath());
    if (suitTex) {
        _suitSprite = Sprite::createWithTexture(suitTex);
        float scale = (W * 0.2f) / _suitSprite->getContentSize().width;
        _suitSprite->setScale(scale);
        _suitSprite->setAnchorPoint(Vec2(1, 1));
        _suitSprite->setPosition(W - 5, H - 5);
        _suitSprite->setLocalZOrder(2);
        addChild(_suitSprite);
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* t, Event*) {
        if (!_isClickable) return false;
        if (getBoundingBox().containsPoint(t->getLocation())) {
            setScale(0.95f);
            return true;
        }
        return false;
        };
    listener->onTouchEnded = [this](Touch*, Event*) {
        setScale(1.0f);
        if (_clickCallback && _model) _clickCallback(_model);
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    updateView();
    return true;
}

void CardView::updateView() {
    bool open = _model->isOpen();
    if (_bgSprite) _bgSprite->setVisible(open);
    if (_bigSprite) _bigSprite->setVisible(open);
    if (_smallSprite) _smallSprite->setVisible(open);
    if (_suitSprite) _suitSprite->setVisible(open);
    if (_backSprite) _backSprite->setVisible(!open);
    _isClickable = open;
}

void CardView::setClickCallback(const std::function<void(CardModel*)>& cb) { _clickCallback = cb; }

std::string CardView::getBigImagePath() const {
    if (!_model) return "";
    return "cards/big_" + std::string(_model->getColor() == Color3B::RED ? "red_" : "black_")
        + _model->getFaceString() + ".png";
}
std::string CardView::getSmallImagePath() const {
    if (!_model) return "";
    return "cards/small_" + std::string(_model->getColor() == Color3B::RED ? "red_" : "black_")
        + _model->getFaceString() + ".png";
}
std::string CardView::getSuitIconPath() const {
    if (!_model) return "";
    static const char* names[] = { "club.png","diamond.png","heart.png","spade.png" };
    return "cards/" + std::string(names[_model->getSuit()]);
}
std::string CardView::getBackImagePath() const {
    return "cards/card_back.png";
}