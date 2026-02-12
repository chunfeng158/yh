#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "models/GameModel.h"
#include <string>

/**
 * @class LevelLoader
 * @brief 关卡配置加载服务
 */
class LevelLoader {
public:
    static GameModel* load(const std::string& filename);
};

#endif