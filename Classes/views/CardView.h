#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "models/CardModel.h"

/**
 * @class CardView
 * @brief 卡牌视图类，继承自cocos2d::Sprite
 *
 * 职责：
 * - 负责卡牌的视觉表现（正面/背面、中央大图、左上角小图、右上角花色图标）
 * - 处理触摸事件，并通过回调通知控制器
 * - 与CardModel绑定，根据模型状态更新显示
 *
 * 使用场景：
 * - 由GameController创建，并添加到场景中
 * - 通过setClickCallback绑定点击事件处理器
 */
class CardView : public cocos2d::Sprite {
public:
    /**
     * @enum CardSize
     * @brief 卡牌尺寸枚举（当前仅使用SIZE_BIG）
     */
    enum CardSize { SIZE_BIG };

    /**
     * @brief 静态工厂方法，创建并初始化CardView实例
     * @param model 关联的CardModel指针
     * @param size 卡牌尺寸（默认SIZE_BIG）
     * @return 自动释放的CardView实例，失败返回nullptr
     */
    static CardView* create(CardModel* model, CardSize size = SIZE_BIG);

    /**
     * @brief 使用模型和尺寸初始化视图
     * @param model 关联的CardModel
     * @param size 卡牌尺寸
     * @return 是否初始化成功
     */
    bool initWithModel(CardModel* model, CardSize size);

    /**
     * @brief 根据模型状态更新视图显示（正面/背面）
     */
    void updateView();

    CardModel* getModel() const { return _model; }
    CardSize getCardSize() const { return _size; }

    /**
     * @brief 设置卡牌点击回调
     * @param callback 接收CardModel*的回调函数
     */
    void setClickCallback(const std::function<void(CardModel*)>& callback);

private:
    // 触摸事件响应
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    // 资源路径生成
    std::string getBigImagePath() const;    // 中央大图路径
    std::string getSmallImagePath() const;  // 左上角小图路径
    std::string getSuitIconPath() const;    // 右上角花色图标路径
    std::string getBackImagePath() const;   // 背面图片路径

    CardModel* _model;                     // 绑定的数据模型
    CardSize _size;                       // 尺寸类型
    cocos2d::Sprite* _bgSprite;          // 正面背景（card_back.png纹理）
    cocos2d::Sprite* _bigSprite;         // 中央大图
    cocos2d::Sprite* _smallSprite;       // 左上角小图
    cocos2d::Sprite* _suitSprite;        // 右上角花色图标
    cocos2d::Sprite* _backSprite;        // 背面精灵
    std::function<void(CardModel*)> _clickCallback; // 点击回调
    bool _isClickable;                  // 当前是否可点击（仅翻开状态可点击）
};

#endif