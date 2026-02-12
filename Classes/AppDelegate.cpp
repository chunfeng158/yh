#include "AppDelegate.h"
#include "services/LevelLoader.h"
#include "controllers/GameController.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

AppDelegate::AppDelegate() : _gameController(nullptr) {}
AppDelegate::~AppDelegate() {
    if (_gameController) {
        _gameController->release();
        _gameController = nullptr;
    }
}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs attrs = { 8,8,8,8,24,8,0 };
    GLView::setGLContextAttrs(attrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::createWithRect("CardGame", Rect(0, 0, 540, 1040));
        director->setOpenGLView(glview);
    }
    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60);

    auto scene = Scene::create();

    // 背景与分割线
    auto bg = LayerColor::create(Color4B(0, 80, 0, 255), 1080, 2080);
    bg->setPosition(Vec2::ZERO);
    scene->addChild(bg);

    const int PLAYFIELD_BOTTOM = 950;   // 主牌区与堆牌区分割线 y 坐标
    auto playfieldBg = LayerColor::create(Color4B(0, 0, 255, 30), 1080, 2080 - PLAYFIELD_BOTTOM);
    playfieldBg->setPosition(Vec2(0, PLAYFIELD_BOTTOM));
    scene->addChild(playfieldBg);

    auto stackBg = LayerColor::create(Color4B(255, 0, 0, 20), 1080, PLAYFIELD_BOTTOM);
    stackBg->setPosition(Vec2(0, 0));
    scene->addChild(stackBg);

    auto line = DrawNode::create();
    line->drawLine(Vec2(0, PLAYFIELD_BOTTOM), Vec2(1080, PLAYFIELD_BOTTOM), Color4F::WHITE);
    scene->addChild(line);

    // 加载关卡
    GameModel* model = LevelLoader::load("level.json");
    if (!model) return false;

    // 计算主牌区整体偏移，使其全部位于分割线上方
    float minY = 9999;
    for (auto card : model->getAllPlayFieldCards()) {
        if (card->getPosition().y < minY) minY = card->getPosition().y;
    }
    int yOffset = PLAYFIELD_BOTTOM - minY + 250;  // 边距50
    if (yOffset < 0) yOffset = 0;
    for (auto card : model->getAllPlayFieldCards()) {
        auto pos = card->getPosition();
        card->setPosition(Vec2(pos.x, pos.y + yOffset));
    }

    // 分离堆牌区与顶牌：最后一张作为初始顶牌
    auto& stackCards = model->getAllStackCards();
    if (!stackCards.empty()) {
        CardModel* topCard = stackCards.back();
        model->removeStackCard((int)stackCards.size() - 1);
        model->setTopCard(topCard);
    }

    // 创建控制器并持久持有
    _gameController = GameController::create();
    _gameController->retain();
    _gameController->setGameModel(model);
    _gameController->setStackBasePos(Vec2(100, 300));
    _gameController->setTopCardFixedPos(Vec2(820, 300));
    _gameController->attach(scene);

    // 回退按钮
    auto undoBtn = Button::create();
    undoBtn->setTitleText("回退");
    undoBtn->setTitleFontSize(36);
    undoBtn->setPosition(Vec2(900, 1950));
    undoBtn->setContentSize(Size(120, 60));
    undoBtn->setColor(Color3B(100, 100, 255));
    undoBtn->addClickEventListener([this](Ref*) {
        if (_gameController) _gameController->undo();
        });
    scene->addChild(undoBtn);

    director->runWithScene(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground() { Director::getInstance()->stopAnimation(); }
void AppDelegate::applicationWillEnterForeground() { Director::getInstance()->startAnimation(); }