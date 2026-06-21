#include "SceneSetup.h"

#include "Core/Resource.h"
#include "Core/EntityFactory.h"
#include "Component/Tags.h"
#include "Component/Panel.h"

#include "Component/NeedleIndex.h"

void SceneSetup::build(entt::registry& reg, Resource& res) {
    auto& f = res.factory;

    // 数据实体
    f.createSessionState(3);
    f.createNextSessionConfig();
    f.createBestRecord();
    f.createSettings();

    // 背景
    f.createBackground();

    // 三根柱子
    f.createNeedle(260, 360, 0);
    f.createNeedle(640, 360, 1);
    f.createNeedle(1020, 360, 2);

    // 底部按钮栏
    auto leftArrow  = f.createInstantButton(200, 680, 30, 30, 3);
    reg.emplace<DiskCountArrowDownTag>(leftArrow);
    auto reset     = f.createInstantButton(400, 680, 60, 30, 3);
    reg.emplace<ResetButtonTag>(reset);
    auto autoDemo  = f.createToggleButton(500, 680, 60, 30, 3);
    reg.emplace<AutoDemoButtonTag>(autoDemo);
    auto history   = f.createInstantButton(600, 680, 50, 30, 3);
    reg.emplace<HistoryPanelToggleTag>(history);
    auto settings  = f.createInstantButton(680, 680, 50, 30, 3);
    reg.emplace<SettingsPanelToggleTag>(settings);
    auto rightArrow = f.createInstantButton(760, 680, 30, 30, 3);
    reg.emplace<DiskCountArrowUpTag>(rightArrow);
    auto closeBtn  = f.createInstantButton(1250, 20, 24, 24, 3);
    reg.emplace<CloseButtonTag>(closeBtn);

    // 文字
    f.createText(40, 680, "Steps:", 20, LIGHTGRAY, 3);
    f.createTextWithTag<StepCounterTag>(100, 680, "0", 20, WHITE, 3);
    f.createText(1140, 680, "Best:", 20, LIGHTGRAY, 3);
    f.createTextWithTag<BestCounterTag>(1200, 680, "--", 20, WHITE, 3);
    f.createTextWithTag<DiskCountTag>(640, 695, "5", 28, WHITE, 3);


    // 面板 + X 按钮
    auto historyP  = f.createPanel(1600.0f, 360.0f, 600.0f, 500.0f, 4, PanelType::History);
    auto historyX = f.createInstantButton(1280 + 260, 140, 20, 20, 5);
    reg.emplace<PanelCloseTag>(historyX);
    
    auto settingsP = f.createPanel(1600.0f, 360.0f, 600.0f, 500.0f, 4, PanelType::Settings);
    auto settingsX = f.createInstantButton(1280 + 260, 140, 20, 20, 5);
    reg.emplace<PanelCloseTag>(settingsX);


    // 遮罩
    f.createOverlay(-640.0f, 360.0f, 1280.0f, 720.0f, 6);

    // 找左柱 → 放初始盘子
    auto needleView = reg.view<NeedleIndex>();
    entt::entity leftNeedle = entt::null;
    for (auto [entity, idx] : needleView.each()) {
        if (idx.index == 0) { leftNeedle = entity; break; }
    }
    if (leftNeedle != entt::null) {
        f.createDisksOnNeedle(leftNeedle);
    }


    // 关卡状态 & 最佳记录


    // 设置

}