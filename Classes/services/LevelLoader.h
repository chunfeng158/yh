#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "models/GameModel.h"
#include <string>

/**
 * @class LevelLoader
 * @brief 关卡配置加载服务（无状态，纯静态类）
 *
 * 职责：
 * - 从 level.json 文件中解析卡牌配置
 * - 根据配置生成 GameModel 实例
 *
 * JSON格式：
 * - Playfield : 主牌区卡牌数组，包含 CardFace、CardSuit、Position
 * - Stack     : 堆牌区卡牌数组，包含 CardFace、CardSuit（Position暂用不到）
 *
 * 使用场景：
 * - 游戏启动时由 AppDelegate 调用
 */
class LevelLoader {
public:
    /**
     * @brief 加载关卡文件，生成GameModel
     * @param filename 配置文件名（如"level.json"）
     * @return 新创建的GameModel指针，调用者负责释放
     */
    static GameModel* load(const std::string& filename);
};

#endif