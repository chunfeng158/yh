#ifndef _APP_DELEGATE_H_
#define _APP_DELEGATE_H_

#include "cocos2d.h"

class GameController;  // 前向声明

/**
 * @class AppDelegate
 * @brief 应用程序入口委托类
 *
 * 职责：
 * - 初始化窗口、设计分辨率、导演
 * - 加载关卡配置，创建GameModel和GameController
 * - 持有GameController实例，防止其被自动释放
 * - 响应应用进入前后台事件
 *
 * 使用场景：
 * - Cocos2d-x 应用程序生命周期管理
 */
class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();
    virtual bool applicationDidFinishLaunching();
    virtual void applicationDidEnterBackground();
    virtual void applicationWillEnterForeground();

private:
    GameController* _gameController;  // 持有的控制器实例
};

#endif // _APP_DELEGATE_H_