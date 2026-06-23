#include "SceneSetup.h"

#include "Core/Resource.h"
#include "Core/EntityFactory.h"
#include "Component/Tags.h"
#include "Component/Panel.h"

#include "Component/NeedleIndex.h"
#include "Component/SessionState.h"
#include "Component/BestRecord.h"
#include "Component/Position.h"
#include "Component/NeedleStack.h"

void SceneSetup::build(entt::registry& reg, Resource& res) {
    auto& f = res.factory;
    auto& cache = res.setupCache;

    // 数据实体
    auto sessionEntity = f.createSessionState(cache.diskCount);
    f.createNextSessionConfig(cache.nextDiskCount);
    f.createBestRecord();
    f.createSettings();

    auto& session = reg.get<SessionState>(sessionEntity);
    session.stepCount  = cache.stepCount;
    session.isAutoDemo = cache.isAutoDemo;
    session.completed  = cache.completed;

    auto bestView = reg.view<BestRecord>();
    if (bestView.begin() != bestView.end()) {
        reg.get<BestRecord>(*bestView.begin()).record = cache.bestRecords;
    }

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
    f.createText(38, 530, "Steps:", 30, LIGHTGRAY, 3);
    f.createTextWithTag<StepCounterTag>(150, 530, std::to_string(cache.stepCount), 30, WHITE, 3);
    f.createText(38, 570, "Best:", 30, LIGHTGRAY, 3);
    f.createTextWithTag<BestCounterTag>(140, 570, 
        (cache.bestRecords[cache.diskCount] == 0) ?
        "--" :
        std::to_string(cache.bestRecords[cache.diskCount]), 30, WHITE, 3);

    f.createTextWithTag<DiskCountTag>(125, 642, std::to_string(cache.diskCount), 36, BLUE, 3);
    
    // 设置面板
    const float panelX = 2560.0f;
    const float panelY = 360.0f;

    auto settingsP = f.createPanel(panelX, panelY, 1280.0f, 720.0f, 4, PanelType::Settings);
    auto settingsX = f.createInstantButton(1280 + 260, 140, 20, 20, 5);
    reg.emplace<PanelCloseTag>(settingsX);

    // 指示灯
    auto diskInd = f.createIndicator(135, 620, 3);
    reg.emplace<DiskCountIndicatorTag>(diskInd);

    auto autoInd = f.createIndicator(290, 620, 3);
    reg.emplace<AutoDemoIndicatorTag>(autoInd);


    // 遮罩
    f.createOverlay(-640.0f, 360.0f, 1280.0f, 720.0f, 6);

    // 找左柱 → 放初始盘子
    auto needleView = reg.view<NeedleIndex>();
    for (auto [entity, idx] : needleView.each()) {
        auto& stack = reg.get<NeedleStack>(entity);
        stack.disks.clear();

        auto& needlePos = reg.get<Position>(entity);
        auto& diskSizes = cache.needles[idx.index].diskSizes;

        for (int i = 0; i < (int)diskSizes.size(); i++) {
            int diskIndex = diskSizes[i];   // cache 存的是 diskSize，即 diskIndex
            float diskY = needlePos.y + res.diskBaseOffset - i * res.diskHeight;
            auto disk = f.createDisk(needlePos.x, diskY, diskIndex, cache.diskCount);
            stack.disks.push_back(disk);
        }
    }

    // 设置
    // 音量行
    // auto volPlainText = f.createText(panelX - 370, panelY - 150, "Volume:", 30, WHITE, 5);
    // reg.emplace<SettingsPanelChildTag>(volPlainText);
    // auto volDown = f.createInstantButton(panelX + 440, panelY - 150, 40, 40, 5);
    // reg.emplace<VolumeDownTag>(volDown);
    // reg.emplace<SettingsPanelChildTag>(volDown);
    // auto volText = f.createTextWithTag<VolumeDisplayTag>(panelX + 470, panelY - 150, "100%", 18, WHITE, 5);
    // reg.emplace<SettingsPanelChildTag>(volText);
    // auto volUp   = f.createInstantButton(panelX + 510, panelY - 150, 40, 40, 5);
    // reg.emplace<VolumeUpTag>(volUp);
    // reg.emplace<SettingsPanelChildTag>(volUp);

    // 分辨率行
    // auto ResolutionPlainText = f.createText(panelX - 220, panelY - 100, "Resolution:", 30, WHITE, 5);
    // reg.emplace<SettingsPanelChildTag>(ResolutionPlainText);
    // auto resPrev = f.createInstantButton(panelX + 100, panelY - 100, 40, 40, 5);
    // reg.emplace<ResolutionPrevTag>(resPrev);
    // reg.emplace<SettingsPanelChildTag>(resPrev);
    // auto resText = f.createTextWithTag<ResolutionDisplayTag>(panelX - 60, panelY - 100, "1280x720", 28, WHITE, 5);
    // reg.emplace<SettingsPanelChildTag>(resText);
    // auto resNext = f.createInstantButton(panelX + 150, panelY - 100, 40, 40, 5);
    // reg.emplace<ResolutionNextTag>(resNext);
    // reg.emplace<SettingsPanelChildTag>(resNext);

    // 删除记录
    // auto delBtn = f.createInstantButton(panelX, panelY + 240, 100, 30, 5);
    // reg.emplace<DeleteBestRecordTag>(delBtn);
    // reg.emplace<SettingsPanelChildTag>(delBtn);

    // 确认 + 取消
    // auto confirm = f.createInstantButton(panelX - 50, panelY + 150, 50, 50, 5);
    // reg.emplace<SettingsConfirmTag>(confirm);
    // reg.emplace<SettingsPanelChildTag>(confirm);
    // auto cancel  = f.createInstantButton(panelX + 50, panelY + 150, 50, 50, 5);
    // reg.emplace<SettingsCancelTag>(cancel);
    // reg.emplace<SettingsPanelChildTag>(cancel);

    auto namePlainText = f.createText(panelX - 130, panelY - 35, "HanoiP83", 72, WHITE, 5);
    reg.emplace<SettingsPanelChildTag>(namePlainText);
    reg.emplace<SettingsPanelChildTag>(namePlainText);

    // X按钮
    auto exit = f.createInstantButton(panelX + 580, panelY + 300, 50, 50, 5);
    reg.emplace<PanelCloseTag>(exit);
    reg.emplace<SettingsPanelChildTag>(exit);
}