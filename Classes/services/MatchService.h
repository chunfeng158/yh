#ifndef MATCH_SERVICE_H
#define MATCH_SERVICE_H

#include "models/CardModel.h"

/**
 * @class MatchService
 * @brief 匹配规则服务（无状态，纯静态类）
 *
 * 职责：
 * - 提供判断两张卡牌是否满足匹配规则的方法
 *
 * 当前规则：
 * - 两张牌的点数绝对值差为1
 *
 * 扩展性：
 * - 如需修改匹配规则，只需修改此文件
 */
class MatchService {
public:
    /**
     * @brief 判断两张牌是否可匹配
     * @param a 卡牌A
     * @param b 卡牌B
     * @return true 当 |a点数 - b点数| == 1
     */
    static bool canMatch(CardModel* a, CardModel* b) {
        return abs(a->getFaceValue() - b->getFaceValue()) == 1;
    }
};

#endif