#include "GameModel.h"

GameModel::GameModel() : _topCard(nullptr) {}
GameModel::~GameModel() {}

// 主牌区
void GameModel::addPlayFieldCard(CardModel* card) { _playFieldCards.push_back(card); }
void GameModel::removePlayFieldCard(int index) {
    if (index >= 0 && index < (int)_playFieldCards.size())
        _playFieldCards.erase(_playFieldCards.begin() + index);
}
void GameModel::removePlayFieldCard(CardModel* card) {
    for (auto it = _playFieldCards.begin(); it != _playFieldCards.end(); ++it) {
        if (*it == card) { _playFieldCards.erase(it); break; }
    }
}
CardModel* GameModel::getPlayFieldCard(int index) const {
    return (index >= 0 && index < (int)_playFieldCards.size()) ? _playFieldCards[index] : nullptr;
}
int GameModel::getPlayFieldCardIndex(CardModel* card) const {
    for (size_t i = 0; i < _playFieldCards.size(); ++i)
        if (_playFieldCards[i] == card) return (int)i;
    return -1;
}

// 堆牌区
void GameModel::addStackCard(CardModel* card) { _stackCards.push_back(card); }
void GameModel::removeStackCard(int index) {
    if (index >= 0 && index < (int)_stackCards.size())
        _stackCards.erase(_stackCards.begin() + index);
}
void GameModel::removeStackCard(CardModel* card) {
    for (auto it = _stackCards.begin(); it != _stackCards.end(); ++it) {
        if (*it == card) { _stackCards.erase(it); break; }
    }
}
CardModel* GameModel::getStackCard(int index) const {
    return (index >= 0 && index < (int)_stackCards.size()) ? _stackCards[index] : nullptr;
}
CardModel* GameModel::getTopStackCard() const {
    return _stackCards.empty() ? nullptr : _stackCards.back();
}
int GameModel::getStackCardIndex(CardModel* card) const {
    for (size_t i = 0; i < _stackCards.size(); ++i)
        if (_stackCards[i] == card) return (int)i;
    return -1;
}

// 顶牌区
void GameModel::setTopCard(CardModel* card) { _topCard = card; }

void GameModel::clear() {
    _playFieldCards.clear();
    _stackCards.clear();
    _topCard = nullptr;
}