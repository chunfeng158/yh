#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>

/**
 * @class GameModel
 * @brief 游戏全局状态数据模型
 *
 * 职责：
 * - 存储所有游戏卡牌的动态数据（主牌区、堆牌区、顶牌）
 * - 提供对卡牌集合的增删改查接口
 * - 不包含任何视图和业务逻辑，仅作为数据容器
 *
 * 使用场景：
 * - 由 LevelLoader 创建并填充
 * - 由 GameController 持有，作为游戏状态的唯一数据源
 */
class GameModel {
public:
    GameModel();
    ~GameModel();

    // ---------- 主牌区操作 ----------
    /**
     * @brief 添加一张牌到主牌区
     * @param card CardModel指针，调用方负责管理内存
     */
    void addPlayFieldCard(CardModel* card);

    /**
     * @brief 从主牌区移除指定索引的牌
     * @param index 索引（从0开始）
     */
    void removePlayFieldCard(int index);

    /**
     * @brief 从主牌区移除指定的牌（通过指针比较）
     * @param card 要移除的卡牌指针
     */
    void removePlayFieldCard(CardModel* card);

    /**
     * @brief 获取主牌区指定索引的牌
     * @param index 索引
     * @return CardModel* 或 nullptr（越界时）
     */
    CardModel* getPlayFieldCard(int index) const;

    /**
     * @brief 获取主牌区所有牌的只读引用
     */
    const std::vector<CardModel*>& getAllPlayFieldCards() const { return _playFieldCards; }

    /**
     * @brief 获取主牌区卡牌数量
     */
    int getPlayFieldCardCount() const { return (int)_playFieldCards.size(); }

    /**
     * @brief 查找指定卡牌在主牌区的索引
     * @param card 目标卡牌指针
     * @return 索引（0-based），-1表示未找到
     */
    int getPlayFieldCardIndex(CardModel* card) const;

    // ---------- 堆牌区操作（堆叠效果）----------
    /**
     * @brief 添加一张牌到堆牌区（自动加入末尾，作为顶部牌）
     * @param card CardModel指针
     */
    void addStackCard(CardModel* card);

    /**
     * @brief 从堆牌区移除指定索引的牌
     * @param index 索引（从0开始）
     */
    void removeStackCard(int index);

    /**
     * @brief 从堆牌区移除指定的牌（通过指针比较）
     * @param card 要移除的卡牌指针
     */
    void removeStackCard(CardModel* card);

    /**
     * @brief 获取堆牌区指定索引的牌
     * @param index 索引
     * @return CardModel* 或 nullptr
     */
    CardModel* getStackCard(int index) const;

    /**
     * @brief 获取堆牌区最上面一张牌（列表末尾）
     * @return 顶部牌指针，若无牌则返回nullptr
     */
    CardModel* getTopStackCard() const;

    /**
     * @brief 获取堆牌区所有牌的只读引用
     */
    const std::vector<CardModel*>& getAllStackCards() const { return _stackCards; }

    /**
     * @brief 获取堆牌区卡牌数量
     */
    int getStackCardCount() const { return (int)_stackCards.size(); }

    /**
     * @brief 查找指定卡牌在堆牌区的索引
     * @param card 目标卡牌指针
     * @return 索引（0-based），-1表示未找到
     */
    int getStackCardIndex(CardModel* card) const;

    /**
     * @brief 设置堆牌区基准位置（最下面一张牌的位置）
     * @param pos 基准坐标（设计分辨率）
     */
    void setStackBasePos(const cocos2d::Vec2& pos) { _stackBasePos = pos; }
    cocos2d::Vec2 getStackBasePos() const { return _stackBasePos; }

    // ---------- 顶牌区操作（固定位置）----------
    /**
     * @brief 设置当前顶牌
     * @param card 顶牌指针
     */
    void setTopCard(CardModel* card);
    CardModel* getTopCard() const { return _topCard; }

    /**
     * @brief 设置顶牌固定显示位置（设计分辨率坐标）
     * @param pos 固定位置
     */
    void setTopCardFixedPos(const cocos2d::Vec2& pos) { _topCardFixedPos = pos; }
    cocos2d::Vec2 getTopCardFixedPos() const { return _topCardFixedPos; }

    /**
     * @brief 清空所有数据（仅清空容器，不释放CardModel内存）
     */
    void clear();

private:
    std::vector<CardModel*> _playFieldCards;   // 主牌区卡牌列表
    std::vector<CardModel*> _stackCards;       // 堆牌区卡牌列表（索引0=最下，back=最上）
    CardModel* _topCard;                       // 当前顶牌指针
    cocos2d::Vec2 _stackBasePos;              // 堆牌区基准位置
    cocos2d::Vec2 _topCardFixedPos;           // 顶牌固定位置
};

#endif