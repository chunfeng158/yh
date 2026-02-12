#include "CardModel.h"

int CardModel::_nextId = 1;

CardModel::CardModel(CardFace face, CardSuit suit, bool isOpen)
    : _id(_nextId++), _face(face), _suit(suit), _isOpen(isOpen) {
}

std::string CardModel::getFaceString() const {
    switch (_face) {
    case FACE_ACE: return "A";
    case FACE_JACK: return "J";
    case FACE_QUEEN: return "Q";
    case FACE_KING: return "K";
    default: return std::to_string(_face);
    }
}

cocos2d::Color3B CardModel::getColor() const {
    return (_suit == SUIT_HEARTS || _suit == SUIT_DIAMONDS)
        ? cocos2d::Color3B::RED
        : cocos2d::Color3B::BLACK;
}