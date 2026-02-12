#ifndef MATCH_SERVICE_H
#define MATCH_SERVICE_H

#include "models/CardModel.h"

/**
 * @class MatchService
 * @brief 匹配规则服务（无状态）
 */
class MatchService {
public:
    static bool canMatch(CardModel* a, CardModel* b) {
        return abs(a->getFaceValue() - b->getFaceValue()) == 1;
    }
};

#endif