#include "App.h"

#include "raylib.h"

#include "System/TestRenderSystem.h"
#include "System/InputSystem.h"
#include "System/ButtonSystem.h"
#include "System/TweenSystem.h"

#include "Component/Position.h"
#include "Component/StaticSprite.h"

App::App()
    : reg(), res(reg), scheduler(reg, res) {
    //注册System
    scheduler.add(std::make_unique<InputSystem>()); // 必须第一个首先添加！
    scheduler.add(std::make_unique<ButtonSystem>());

    scheduler.add(std::make_unique<TweenSystem>());
    scheduler.enterScene("gameplay");
    
}

void App::run() {

    while (!WindowShouldClose()) {

        //检查窗口更新
        if (IsWindowResized()) {
            res.windowState.update();
        }
        
        float frameTime = GetFrameTime();
        res.dt = frameTime;

        // 固定步长循环
        res.accumulator += frameTime;
        if (res.accumulator > 0.2f) res.accumulator = 0.2f;  // 防螺旋

        // - 执行逻辑系统 - 
        res.events.update(); //刷新事件
        scheduler.runLogic();

        // - 执行固定步长系统 - 
        while (res.accumulator >= res.fixedDt) {
            scheduler.runFixedUpdate();
            res.accumulator -= res.fixedDt;
        }

        // 插值因子 = 剩余时间占固定步长的比例
        res.alpha = res.accumulator / res.fixedDt;

        // 渲染
        BeginTextureMode(res.windowState.target);
        ClearBackground(BLACK);
        // - 执行渲染系统 -
        scheduler.runRender();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            res.windowState.target.texture,
            {0, 0, (float)res.windowState.logicWidth, -(float)res.windowState.logicHeight},
            {res.windowState.offset.x, res.windowState.offset.y,
             res.windowState.logicWidth * res.windowState.scale, res.windowState.logicHeight * res.windowState.scale},
            {0, 0}, 0, WHITE
        );
        EndDrawing();
    }

}