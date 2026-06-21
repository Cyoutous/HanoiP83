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
    f.createNextSessionConfig(3);
    f.createBestRecord();
    f.createSettings();

    // 背景
    f.createBackground();

    // 三根柱子
    f.createNeedle(350, 340, 0);
    f.createNeedle(640, 340, 1);
    f.createNeedle(930, 340, 2);

    // 底部按钮栏
    auto leftArrow  = f.createInstantButton(60, 660, 50, 50, 3);
        reg.emplace<DiskCountArrowDownTag>(leftArrow);
    auto rightArrow = f.createInstantButton(210, 660, 50, 50, 3);
        reg.emplace<DiskCountArrowUpTag>(rightArrow);
    auto autoDemo  = f.createToggleButton(290, 660, 50, 50, 3);
        reg.emplace<AutoDemoButtonTag>(autoDemo);
    auto reset     = f.createInstantButton(370, 660, 50, 50, 3);
        reg.emplace<ResetButtonTag>(reset);
    // auto history   = f.createInstantButton(600, 680, 50, 30, 3);
    //     reg.emplace<HistoryPanelToggleTag>(history);
    auto settings  = f.createInstantButton(1220, 660, 50, 50, 3);
        reg.emplace<SettingsPanelToggleTag>(settings);

    // 文字
    f.createText(40, 620, "Steps:", 20, LIGHTGRAY, 3);
    f.createTextWithTag<StepCounterTag>(110, 620, "0", 20, WHITE, 3);
    f.createText(40, 590, "Best:", 20, LIGHTGRAY, 3);
    f.createTextWithTag<BestCounterTag>(110, 590, "--", 20, WHITE, 3);

    f.createTextWithTag<DiskCountTag>(125, 642, "3", 36, BLUE, 3);


    // 面板 + X 按钮
    // auto historyP  = f.createPanel(1600.0f, 360.0f, 600.0f, 500.0f, 4, PanelType::History);
    // auto historyX = f.createInstantButton(1280 + 260, 140, 20, 20, 5);
    // reg.emplace<PanelCloseTag>(historyX);
    
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