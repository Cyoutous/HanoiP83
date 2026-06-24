#include "App.h"

#include "raylib.h"

#include "Core/SceneSetup.h"
#include "Core/SaveManager.h"

#include "System/TestRenderSystem.h"
#include "System/InputSystem.h"
#include "System/ButtonSystem.h"
#include "System/SessionStateUpdateSystem.h"
#include "System/NeedleInteractionSystem.h"
#include "System/PanelSystem.h"
#include "System/IndicatorSystem.h"
#include "System/SettingsSystem.h"

#include "System/TweenSystem.h"
#include "System/MoveSnapshotSystem.h"
#include "System/MoveExecutionSystem.h"
#include "System/GamePhaseSystem.h"
#include "System/TimerSystem.h"
#include "System/AutoDemoSystem.h"
#include "System/ResetSystem.h"
#include "System/NextSessionConfigSystem.h"

#include "System/RectRenderSystem.h"
#include "System/SpriteRenderSystem.h"
#include "System/RenderSystem.h"
#include "System/UIRenderSystem.h"

#include "Component/Position.h"
#include "Component/StaticSprite.h"
#include "Component/Tags.h"


App::App()
    : reg(), res(reg), scheduler(reg, res) {
    //注册System
    scheduler.add(std::make_unique<InputSystem>()); // 必须第一个首先添加！
    scheduler.add(std::make_unique<ButtonSystem>());
    scheduler.add(std::make_unique<SessionStateUpdateSystem>());
    scheduler.add(std::make_unique<NeedleInteractionSystem>());
    scheduler.add(std::make_unique<PanelSystem>());
    scheduler.add(std::make_unique<IndicatorSystem>());
    scheduler.add(std::make_unique<SettingsSystem>());
    
    scheduler.add(std::make_unique<TweenSystem>());
    scheduler.add(std::make_unique<MoveSnapshotSystem>());
    scheduler.add(std::make_unique<MoveExecutionSystem>());
    scheduler.add(std::make_unique<GamePhaseSystem>());
    scheduler.add(std::make_unique<TimerSystem>());
    scheduler.add(std::make_unique<AutoDemoSystem>());
    scheduler.add(std::make_unique<ResetSystem>());
    scheduler.add(std::make_unique<NextSessionConfigSystem>());
    
    scheduler.add(std::make_unique<RectRenderSystem>());
    // scheduler.add(std::make_unique<SpriteRenderSystem>());
    //scheduler.add(std::make_unique<UIRenderSystem>());
    scheduler.add(std::make_unique<RenderSystem>());
    
    //scheduler.add(std::make_unique<>());
    
    SaveManager::load(res);
    SceneSetup::build(reg, res);
    scheduler.enterScene("gameplay");
    
}

void App::run() {

    // 第一次启动打开说明面板
    if (!SaveManager::hasSave()) {
        auto btnView = reg.view<SettingsPanelToggleTag>();
        if (btnView.begin() != btnView.end()) {
            res.events.trigger(ButtonClickedEvent{*btnView.begin()});
        }
    }

    while (!WindowShouldClose()) {

        //检查窗口更新
        if (IsWindowResized()) {
            int w = GetScreenWidth();
            int h = GetScreenHeight();
            float targetRatio = 1280.0f / 720.0f;
            float actualRatio = (float)w / h;

            if (actualRatio > targetRatio) {
                // 太宽 → 收缩宽度
                w = (int)(h * targetRatio);
            } else {
                // 太高 → 收缩高度
                h = (int)(w / targetRatio);
            }
            SetWindowSize(w, h);

            res.windowState.update();
        }
        
        float frameTime = GetFrameTime();
        res.dt = frameTime;

        // 固定步长循环
        res.accumulator += frameTime;
        if (res.accumulator > 0.2f) res.accumulator = 0.2f;  // 防螺旋

        // - 执行逻辑系统 - 
        scheduler.runLogic();
        res.events.update(); //刷新事件

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

    SaveManager::save(reg);
}