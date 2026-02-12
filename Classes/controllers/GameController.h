#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/CardView.h"
#include "managers/UndoManager.h"
#include <unordered_map>

/**
 * @class GameController
 * @brief 游戏主控制器，继承自cocos2d::Ref以支持自动内存管理
 *
 * 职责：
 * - 持有GameModel（数据模型）、UndoManager（回退管理器）
 * - 处理卡牌点击事件，调度游戏动作（匹配、堆牌→顶牌）
 * - 管理CardView的创建、布局、刷新
 * - 执行撤销操作
 *
 * 使用场景：
 * - 由AppDelegate创建并持有（通过retain）
 * - 在游戏运行时作为唯一的逻辑中枢
 */
class GameController : public cocos2d::Ref {
public:
    /**
     * @brief 静态工厂方法，创建并初始化GameController实例
     * @return 自动释放的GameController实例（调用者需retain以持久持有）
     */
    static GameController* create();
    bool init();

    /**
     * @brief 绑定游戏场景，并刷新所有视图
     * @param scene 主场景指针
     */
    void attach(cocos2d::Node* scene);

    /**
     * @brief 设置游戏数据模型（由外部注入）
     * @param model GameModel指针，控制器不会释放它
     */
    void setGameModel(GameModel* model);

    /**
     * @brief 设置堆牌区基准位置（最下面一张牌的位置）
     * @param pos 设计分辨率下的坐标
     */
    void setStackBasePos(const cocos2d::Vec2& pos);

    /**
     * @brief 设置顶牌固定显示位置
     * @param pos 设计分辨率下的坐标
     */
    void setTopCardFixedPos(const cocos2d::Vec2& pos);

    /**
     * @brief 执行撤销操作（从UndoManager取出记录并恢复状态）
     */
    void undo();

private:
    GameController();
    ~GameController();

    // 事件处理
    void onCardClicked(CardModel* card);

    // 游戏动作
    void performMatch(CardModel* playFieldCard);      // 主牌区匹配
    void performStackToTop(CardModel* stackCard);     // 堆牌区→顶牌

    // 视图管理
    void refreshAllViews();                           // 完全刷新所有视图
    void refreshStackViews();                         // 仅刷新堆牌区位置和回调
    CardView* createCardView(CardModel* model);       // 创建单个卡牌视图（并存入映射）

    // 撤销动作
    void undoMoveCard(const UndoRecord& rec);
    void undoStackToTop(const UndoRecord& rec);

    cocos2d::Node* _scene;                           // 主场景指针
    GameModel* _model;                              // 游戏数据模型（不负责释放）
    UndoManager* _undoManager;                      // 回退管理器
    std::unordered_map<int, CardView*> _cardViewMap; // 卡牌ID -> CardView 映射，用于快速查找
    bool _isAnimating;                             // 是否正在播放动画（阻止并发操作）
    const cocos2d::Vec2 STACK_OFFSET = cocos2d::Vec2(15, -15); // 堆牌区每张牌偏移量（向右下）
};

#endif