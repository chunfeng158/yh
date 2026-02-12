#include "controllers/GameController.h"
#include "services/MatchService.h"

USING_NS_CC;

GameController* GameController::create() {
    auto ret = new GameController();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameController::init() {
    _model = nullptr;
    _scene = nullptr;
    _undoManager = new UndoManager();
    _isAnimating = false;
    return true;
}

GameController::GameController() {}
GameController::~GameController() {
    CC_SAFE_DELETE(_undoManager);
}

void GameController::attach(Node* scene) {
    _scene = scene;
    refreshAllViews();
}

void GameController::setGameModel(GameModel* model) {
    _model = model;
}

void GameController::setStackBasePos(const Vec2& pos) {
    if (_model) _model->setStackBasePos(pos);
}

void GameController::setTopCardFixedPos(const Vec2& pos) {
    if (_model) _model->setTopCardFixedPos(pos);
}

CardView* GameController::createCardView(CardModel* model) {
    auto view = CardView::create(model, CardView::SIZE_BIG);
    view->setClickCallback([this](CardModel* c) { onCardClicked(c); });
    view->setLocalZOrder(0);
    _cardViewMap[model->getId()] = view;
    return view;
}

void GameController::refreshAllViews() {
    if (!_scene || !_model) return;

    for (auto& pair : _cardViewMap) {
        pair.second->removeFromParent();
    }
    _cardViewMap.clear();

    for (auto card : _model->getAllPlayFieldCards()) {
        auto view = createCardView(card);
        view->setPosition(card->getPosition());
        _scene->addChild(view);
    }

    if (_model->getTopCard()) {
        auto view = createCardView(_model->getTopCard());
        view->setPosition(_model->getTopCardFixedPos());
        _scene->addChild(view);
    }

    refreshStackViews();
}

void GameController::refreshStackViews() {
    if (!_scene || !_model) return;

    auto& cards = _model->getAllStackCards();
    auto basePos = _model->getStackBasePos();

    for (size_t i = 0; i < cards.size(); ++i) {
        CardModel* card = cards[i];
        Vec2 pos = basePos + STACK_OFFSET * (int)i;
        card->setPosition(pos);

        CardView* view = nullptr;
        auto it = _cardViewMap.find(card->getId());
        if (it != _cardViewMap.end()) {
            view = it->second;
        }
        else {
            view = createCardView(card);
        }
        view->setPosition(pos);
        view->setLocalZOrder((int)i);

        if (i == cards.size() - 1) {
            view->setClickCallback([this](CardModel* c) { onCardClicked(c); });
        }
        else {
            view->setClickCallback(nullptr);
        }

        if (view->getParent() != _scene) {
            _scene->addChild(view);
        }
    }
}

void GameController::onCardClicked(CardModel* card) {
    if (_isAnimating || !_model || !_model->getTopCard()) return;

    int idx = _model->getPlayFieldCardIndex(card);
    if (idx != -1) {
        if (MatchService::canMatch(card, _model->getTopCard())) {
            performMatch(card);
        }
        else {
            CCLOG("cannot match");
        }
        return;
    }

    if (_model->getTopStackCard() == card) {
        performStackToTop(card);
    }
}

void GameController::performMatch(CardModel* playFieldCard) {
    int idx = _model->getPlayFieldCardIndex(playFieldCard);
    if (idx == -1) return;

    CardView* movedView = _cardViewMap[playFieldCard->getId()];
    CardView* oldTopView = _cardViewMap[_model->getTopCard()->getId()];
    if (!movedView || !oldTopView) return;

    _undoManager->pushMoveRecord(
        playFieldCard->getId(),
        playFieldCard->getPosition(),
        _model->getTopCardFixedPos(),
        _model->getTopCard()->getId(),
        _model->getTopCardFixedPos()
    );

    oldTopView->retain();
    oldTopView->removeFromParent();

    _isAnimating = true;
    Vec2 target = _model->getTopCardFixedPos();

    auto move = MoveTo::create(0.3f, target);
    auto cb = CallFunc::create([=]() {
        _model->removePlayFieldCard(idx);
        _model->setTopCard(playFieldCard);

        movedView->setPosition(target);
        movedView->setClickCallback(nullptr);

        _isAnimating = false;
        CCLOG("match done");
        });
    movedView->runAction(Sequence::create(move, cb, nullptr));
}

void GameController::performStackToTop(CardModel* stackCard) {
    int idx = _model->getStackCardIndex(stackCard);
    if (idx == -1 || idx != _model->getStackCardCount() - 1) return;

    CardView* movedView = _cardViewMap[stackCard->getId()];
    CardView* oldTopView = _cardViewMap[_model->getTopCard()->getId()];
    if (!movedView || !oldTopView) return;

    _undoManager->pushStackToTopRecord(
        stackCard->getId(),
        stackCard->getPosition(),
        _model->getTopCardFixedPos(),
        idx,
        _model->getTopCard()->getId(),
        _model->getTopCardFixedPos()
    );

    oldTopView->retain();
    oldTopView->removeFromParent();

    _model->removeStackCard(idx);

    _isAnimating = true;
    Vec2 target = _model->getTopCardFixedPos();

    auto move = MoveTo::create(0.3f, target);
    auto cb = CallFunc::create([=]() {
        stackCard->setPosition(target);
        movedView->setPosition(target);
        movedView->setClickCallback(nullptr);
        _model->setTopCard(stackCard);

        refreshStackViews();

        _isAnimating = false;
        CCLOG("stack to top done");
        });
    movedView->runAction(Sequence::create(move, cb, nullptr));
}

void GameController::undo() {
    if (_isAnimating || !_undoManager->canUndo()) return;
    auto rec = _undoManager->popRecord();
    CCLOG("undo: type=%s", rec.type == UndoRecord::MOVE_CARD ? "MOVE" : "STACK_TO_TOP");

    if (rec.type == UndoRecord::MOVE_CARD) {
        undoMoveCard(rec);
    }
    else if (rec.type == UndoRecord::STACK_TO_TOP) {
        undoStackToTop(rec);
    }
}

void GameController::undoMoveCard(const UndoRecord& rec) {
    if (!_model->getTopCard() || _model->getTopCard()->getId() != rec.cardId) return;

    CardView* movedView = _cardViewMap[rec.cardId];
    if (!movedView) return;

    _isAnimating = true;
    auto moveBack = MoveTo::create(0.3f, rec.fromPos);
    auto cb = CallFunc::create([=]() {
        movedView->setPosition(rec.fromPos);
        movedView->setClickCallback([this](CardModel* c) { onCardClicked(c); });
        movedView->setLocalZOrder(0);

        CardModel* card = _model->getTopCard();
        _model->setTopCard(nullptr);
        _model->addPlayFieldCard(card);

        auto it = _cardViewMap.find(rec.oldTopId);
        if (it != _cardViewMap.end()) {
            CardView* oldTopView = it->second;
            oldTopView->setPosition(rec.oldTopPos);
            if (oldTopView->getParent() != _scene) {
                _scene->addChild(oldTopView);
            }
            oldTopView->release();
            oldTopView->setClickCallback(nullptr);
            _model->setTopCard(oldTopView->getModel());
        }

        _isAnimating = false;
        CCLOG("undo move done");
        });
    movedView->runAction(Sequence::create(moveBack, cb, nullptr));
}

void GameController::undoStackToTop(const UndoRecord& rec) {
    if (!_model->getTopCard() || _model->getTopCard()->getId() != rec.cardId) return;

    CardView* movedView = _cardViewMap[rec.cardId];
    if (!movedView) return;

    _isAnimating = true;
    auto moveBack = MoveTo::create(0.3f, rec.fromPos);
    auto cb = CallFunc::create([=]() {
        CardModel* stackCard = _model->getTopCard();
        _model->setTopCard(nullptr);

        movedView->setPosition(rec.fromPos);
        movedView->setClickCallback(nullptr);
        _model->addStackCard(stackCard);

        auto it = _cardViewMap.find(rec.oldTopId);
        if (it != _cardViewMap.end()) {
            CardView* oldTopView = it->second;
            oldTopView->setPosition(rec.oldTopPos);
            if (oldTopView->getParent() != _scene) {
                _scene->addChild(oldTopView);
            }
            oldTopView->release();
            oldTopView->setClickCallback(nullptr);
            _model->setTopCard(oldTopView->getModel());
        }

        refreshStackViews();

        _isAnimating = false;
        CCLOG("undo stack to top done");
        });
    movedView->runAction(Sequence::create(moveBack, cb, nullptr));
}