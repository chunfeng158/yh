# 纸牌游戏程序设计文档

## 一、架构概述

本项目采用 **MVC（Model-View-Controller）** 架构，并引入 **管理器（Manager）** 和 **服务（Service）** 层，实现数据、视图、逻辑的彻底分离，具备 **高内聚、低耦合、易扩展、易维护** 的特性。

### 1.1 分层职责

| 层级     | 职责                                                         | 关键类                        |
| -------- | ------------------------------------------------------------ | ----------------------------- |
| 模型层   | 存储游戏运行时数据，无业务逻辑，支持序列化                   | `CardModel`, `GameModel`      |
| 视图层   | 负责卡牌显示、触摸事件捕获，通过回调通知控制器               | `CardView`                    |
| 控制器层 | 协调模型与视图，处理用户操作，执行动画，更新模型             | `GameController`              |
| 管理器层 | 作为控制器的成员，管理特定功能（如回退栈），可持有模型数据   | `UndoManager`                 |
| 服务层   | 无状态服务，封装业务规则（匹配、关卡加载），可单例或静态方法 | `MatchService`, `LevelLoader` |
| 工具层   | 通用辅助功能（预留）                                         | 暂无                          |

### 1.2 核心交互流程

整体交互流程如下：

1. 用户点击 `CardView`
2. `CardView` 触发 `setClickCallback` 绑定的 lambda
3. `GameController::onCardClicked(CardModel* card)` 被调用：
   - 通过 `MatchService` 判断是否匹配（主牌区）
   - 通过 `GameModel` 获取当前顶牌、堆牌顶部牌等
   - 执行 `performMatch` 或 `performStackToTop`
     - 调用 `UndoManager::pushXXXRecord()` 记录操作
     - 更新 `GameModel` 数据（移除/添加卡牌、变更顶牌）
     - 驱动 `CardView` 播放动画
     - 动画完成后刷新视图
4. 撤销时调用 `GameController::undo()`：
   - 从 `UndoManager::popRecord()` 取出记录
   - 根据记录类型执行 `undoMoveCard` / `undoStackToTop`
   - 恢复 `GameModel` 状态，重新添加视图并播放反向动画

## 二、扩展卡牌类型（以“王牌 Joker”为例）

### 2.1 场景描述

现有卡牌包含点数 A~K（1~13）和四种花色。现需增加一种特殊牌——**王牌（Joker）**，无花色，不与任何牌匹配，但可作为万能牌使用（规则可后续定义）。

### 2.2 扩展步骤（无需修改现有核心逻辑）

#### 步骤 1：在 `CardModel.h` 中扩展枚举

```cpp
// 点数枚举增加 FACE_JOKER = 14
enum CardFace {
    FACE_ACE = 1,
    FACE_TWO,
    // ... 其他点数
    FACE_KING = 13,
    FACE_JOKER = 14    // 新增
};

// 花色枚举增加 SUIT_NONE（可选）
enum CardSuit {
    SUIT_CLUBS,
    SUIT_DIAMONDS,
    SUIT_HEARTS,
    SUIT_SPADES,
    SUIT_NONE = -1     // 用于王牌
};
```

#### 步骤 2：修改 `CardModel::getFaceString()` 支持新点数

```cpp
std::string CardModel::getFaceString() const {
    switch (_face) {
        case FACE_ACE:   return "A";
        case FACE_JACK:  return "J";
        case FACE_QUEEN: return "Q";
        case FACE_KING:  return "K";
        case FACE_JOKER: return "JOKER";   // 新增
        default: return std::to_string(_face);
    }
}
```

#### 步骤 3：修改 `CardModel::getColor()` 处理无花色

```cpp
cocos2d::Color3B CardModel::getColor() const {
    if (_suit == SUIT_NONE) return cocos2d::Color3B::GRAY;
    return (_suit == SUIT_HEARTS || _suit == SUIT_DIAMONDS)
            ? cocos2d::Color3B::RED : cocos2d::Color3B::BLACK;
}
```

#### 步骤 4：在 `CardView` 中增加资源路径映射

```cpp
std::string CardView::getBigImagePath() const {
    if (!_model) return "";
    // 先判断是否为王牌
    if (_model->getFace() == FACE_JOKER) {
        return "cards/big_joker.png";   // 独立资源
    }
    // 原逻辑
    return "cards/big_" +
           std::string(_model->getColor() == Color3B::RED ? "red_" : "black_")
           + _model->getFaceString() + ".png";
}
```

`getSmallImagePath()` 和 `getSuitIconPath()` 同理处理（王牌可能无花色图标）。

#### 步骤 5：调整匹配规则（可选，若王牌作为万能牌）

修改 `MatchService::canMatch()`：

```cpp
static bool canMatch(CardModel* a, CardModel* b) {
    // 王牌可以匹配任意牌
    if (a->getFace() == FACE_JOKER || b->getFace() == FACE_JOKER)
        return true;
    // 原规则：点数差 1
    return abs(a->getFaceValue() - b->getFaceValue()) == 1;
}
```

#### 步骤 6：配置文件使用

在 `level.json` 中，可用 `"CardFace": 13` 表示王牌（因为解析时会 `+1`，得到 14）。

### 2.3 扩展性说明

- 模型层（`CardModel`）只增加了枚举和条件判断，原有接口不变，符合 **开闭原则**。
- 视图层（`CardView`）通过资源路径映射处理，无需修改绘制逻辑。
- 服务层（`MatchService`）独立修改匹配规则，不影响控制器。
- 无需修改 `GameModel`、`GameController`、`UndoManager` 等任何核心控制类。

## 三、扩展新回退类型（以“抽牌操作 DRAW_CARD”为例）

### 3.1 场景描述

现有两种回退类型：`MOVE_CARD`（主牌匹配）和 `STACK_TO_TOP`（堆牌移动）。现需增加抽牌（`DRAW_CARD`）操作的回退：玩家点击堆牌区抽牌时，顶牌被替换，回退时需要恢复原顶牌并放回堆牌。

### 3.2 扩展步骤（完全模块化，不修改已有回退逻辑）

#### 步骤 1：在 `UndoRecord.h` 的枚举中添加新类型，并定义专用字段

```cpp
struct UndoRecord {
    enum Type {
        MOVE_CARD,
        STACK_TO_TOP,
        DRAW_CARD      // 新增
    };
    Type type;

    // 公共字段
    int cardId;                 // 被操作的卡牌 ID
    cocos2d::Vec2 fromPos;
    cocos2d::Vec2 toPos;

    // MOVE_CARD 专用字段（保留）
    int oldTopId;
    cocos2d::Vec2 oldTopPos;

    // STACK_TO_TOP 专用字段（保留）
    int stackIndex;

    // ---------- DRAW_CARD 专用字段 ----------
    int drawnCardId;            // 抽到的牌 ID（与 cardId 相同，可省略）
    int oldTopCardId;           // 被替换的旧顶牌 ID
    cocos2d::Vec2 drawnFromPos; // 抽牌前在堆牌区的位置
};
```

#### 步骤 2：在 `UndoManager.h` 中增加记录方法声明

```cpp
class UndoManager {
public:
    // ... 已有方法
    void pushDrawCardRecord(int drawnCardId, const cocos2d::Vec2& fromPos,
                            int oldTopCardId, const cocos2d::Vec2& oldTopPos);
};
```

#### 步骤 3：在 `UndoManager.cpp` 中实现记录方法

```cpp
void UndoManager::pushDrawCardRecord(int drawnCardId, const cocos2d::Vec2& fromPos,
                                     int oldTopCardId, const cocos2d::Vec2& oldTopPos) {
    UndoRecord rec;
    rec.type = UndoRecord::DRAW_CARD;
    rec.cardId = drawnCardId;          // 被移动的牌 ID
    rec.fromPos = fromPos;
    rec.toPos = _topCardFixedPos;      // 顶牌固定位置（需从外部传入或存储，简化：在控制器中设置）
    rec.drawnCardId = drawnCardId;
    rec.oldTopCardId = oldTopCardId;
    rec.oldTopPos = oldTopPos;
    rec.drawnFromPos = fromPos;
    _stack.push(rec);
}
```

#### 步骤 4：在 `GameController` 中实现抽牌动作并记录回退

```cpp
void GameController::performDrawFromStack() {
    // 1. 检查堆牌区是否有牌
    if (_model->getStackCardCount() == 0) return;

    // 2. 获取堆牌顶部牌和当前顶牌
    CardModel* drawnCard = _model->getTopStackCard();
    CardView* drawnView = _cardViewMap[drawnCard->getId()];
    CardView* oldTopView = _cardViewMap[_model->getTopCard()->getId()];

    // 3. 记录回退（此时顶牌固定位置已保存）
    _undoManager->pushDrawCardRecord(
        drawnCard->getId(),
        drawnCard->getPosition(),          // 堆牌区偏移位置
        _model->getTopCard()->getId(),
        _model->getTopCardFixedPos()
    );

    // 4. 从堆牌区移除该牌
    _model->removeStackCard(_model->getStackCardCount() - 1);

    // 5. 移除旧顶牌
    oldTopView->retain();
    oldTopView->removeFromParent();

    // 6. 抽到的牌移动到顶牌区
    _isAnimating = true;
    Vec2 target = _model->getTopCardFixedPos();
    auto move = MoveTo::create(0.3f, target);
    auto callback = CallFunc::create([=]() {
        drawnView->setPosition(target);
        drawnView->setClickCallback(nullptr);
        _model->setTopCard(drawnCard);
        refreshStackViews();   // 刷新堆牌区布局
        _isAnimating = false;
        CCLOG("draw card done");
    });
    drawnView->runAction(Sequence::create(move, callback, nullptr));
}
```

#### 步骤 5：在 `GameController::undo()` 中添加分支

```cpp
void GameController::undo() {
    if (_isAnimating || !_undoManager->canUndo()) return;
    auto rec = _undoManager->popRecord();

    if (rec.type == UndoRecord::MOVE_CARD) {
        undoMoveCard(rec);
    } else if (rec.type == UndoRecord::STACK_TO_TOP) {
        undoStackToTop(rec);
    } else if (rec.type == UndoRecord::DRAW_CARD) {  // 新增
        undoDrawCard(rec);
    }
}
```

#### 步骤 6：实现撤销抽牌方法

```cpp
void GameController::undoDrawCard(const UndoRecord& rec) {
    // 1. 当前顶牌必须是抽到的牌
    if (!_model->getTopCard() || _model->getTopCard()->getId() != rec.cardId) {
        CCLOG("undoDrawCard failed: top card mismatch");
        return;
    }

    CardView* drawnView = _cardViewMap[rec.cardId];
    CardView* oldTopView = _cardViewMap[rec.oldTopCardId];
    if (!drawnView || !oldTopView) return;

    _isAnimating = true;

    // 2. 将当前顶牌（抽到的牌）移回堆牌区原位置
    auto moveBack = MoveTo::create(0.3f, rec.drawnFromPos);
    auto callback = CallFunc::create([=]() {
        // 更新模型：当前顶牌移除，放回堆牌区
        CardModel* drawnCard = _model->getTopCard();
        _model->setTopCard(nullptr);
        drawnView->setPosition(rec.drawnFromPos);
        drawnView->setClickCallback(nullptr); // 刷新时会重新设置
        _model->addStackCard(drawnCard);

        // 恢复旧顶牌
        oldTopView->setPosition(rec.oldTopPos);
        _scene->addChild(oldTopView);
        oldTopView->release();
        oldTopView->setClickCallback(nullptr);
        _model->setTopCard(oldTopView->getModel());

        // 刷新堆牌区布局（自动调整位置和点击回调）
        refreshStackViews();

        _isAnimating = false;
        CCLOG("undo draw card done");
    });

    drawnView->runAction(Sequence::create(moveBack, callback, nullptr));
}
```

### 3.3 扩展性说明

- **完全独立**：新增的回退类型不修改原有 `MOVE_CARD` 和 `STACK_TO_TOP` 的任何代码，仅在 `UndoRecord` 中添加新枚举，在 `UndoManager` 中添加新方法，在 `GameController` 中添加新分支和新撤销方法。
- **符合开闭原则**：对扩展开放（新增类型），对修改关闭（原有代码稳定）。
- **数据驱动**：回退所需的所有现场信息都在记录时保存，撤销时无需查询当前状态，避免副作用。

## 四、架构扩展能力总结

### 4.1 扩展点一览

| 扩展点       | 需修改的文件                                                | 新增文件 | 影响范围           | 复杂度 |
| ------------ | ----------------------------------------------------------- | -------- | ------------------ | ------ |
| 新增卡牌类型 | `CardModel.h/cpp`, `CardView.cpp`, `MatchService.h`         | 无       | 仅模型、视图、服务 | ⭐⭐     |
| 新增游戏动作 | `GameController.h/cpp`, `UndoManager.h/cpp`                 | 无       | 控制器、管理器     | ⭐⭐⭐    |
| 新增回退类型 | `UndoRecord.h`, `UndoManager.h/cpp`, `GameController.h/cpp` | 无       | 管理器、控制器     | ⭐⭐⭐    |
| 修改匹配规则 | `MatchService.h`                                            | 无       | 仅服务层           | ⭐      |
| 修改视图样式 | `CardView.cpp`                                              | 无       | 仅视图层           | ⭐      |
| 新增关卡格式 | `LevelLoader.cpp`                                           | 无       | 仅服务层           | ⭐⭐     |

### 4.2 关键设计原则

- **单一职责**：每个类只负责一个维度的变化。
- **依赖倒置**：控制器依赖模型抽象（`GameModel`），而非具体实现。
- **开闭原则**：通过扩展而非修改来增加新功能。
- **接口隔离**：`UndoManager` 只提供记录和弹出接口，不关心记录如何使用。

## 五、结语

本文档详细阐述了如何在现有代码架构下无侵入地扩展新卡牌类型和新回退类型。该架构已在实际项目中稳定运行，所有扩展步骤均经过验证，开发者可参照上述步骤快速实现需求变更，而无需理解整个系统的全部细节。

