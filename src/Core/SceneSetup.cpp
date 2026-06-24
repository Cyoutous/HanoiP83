#include "SceneSetup.h"

#include "Core/Resource.h"
#include "Core/SaveManager.h"

#include "Core/EntityFactory.h"
#include "Component/Tags.h"
#include "Component/Panel.h"

#include "Component/NeedleIndex.h"
#include "Component/SessionState.h"
#include "Component/BestRecord.h"
#include "Component/Position.h"
#include "Component/NeedleStack.h"

#include "Component/ButtonState.h"
#include "Component/SpriteStates.h"

#include "Event/ButtonClickedEvent.h"

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
    session.elapsedTime  = cache.elapsedTime;
    session.timerRunning = cache.timerRunning;
    session.timeUp       = cache.timeUp;

    auto bestView = reg.view<BestRecord>();
    if (bestView.begin() != bestView.end()) {
        reg.get<BestRecord>(*bestView.begin()).record = cache.bestRecords;
        reg.get<BestRecord>(*bestView.begin()).bestTimes = cache.bestTimes;
    }

    // 背景
    f.createBackground();

    // 三根柱子
    f.createNeedle(350, 310, 0);
    f.createNeedle(640, 310, 1);
    f.createNeedle(930, 310, 2);

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


    // 按钮贴图

    auto& leftArrowButton = reg.emplace<SpriteStates>(leftArrow);
    leftArrowButton.width = 50;
    leftArrowButton.height = 50;
    leftArrowButton.textures[0]                              = LoadTexture("assets/Texture/leftArrow.png");
    leftArrowButton.textures[(int)ButtonVisual::Pressed]     = LoadTexture("assets/Texture/leftArrowPressed.png");

    auto& rightArrowButton = reg.emplace<SpriteStates>(rightArrow);
    rightArrowButton.width = 50;
    rightArrowButton.height = 50;
    rightArrowButton.textures[0]                             = LoadTexture("assets/Texture/rightArrow.png");
    rightArrowButton.textures[(int)ButtonVisual::Pressed]    = LoadTexture("assets/Texture/rightArrowPressed.png");

    auto& autoSS = reg.emplace<SpriteStates>(autoDemo);
    autoSS.width = 50;
    autoSS.height = 50;
    autoSS.textures[0]                                       = LoadTexture("assets/Texture/autoButton.png");
    autoSS.textures[(int)ButtonVisual::Pressed]              = LoadTexture("assets/Texture/autoButtonPressed.png");

    auto& resetSS = reg.emplace<SpriteStates>(reset);
    resetSS.width = 50;
    resetSS.height = 50;
    resetSS.textures[0]                                      = LoadTexture("assets/Texture/resetButton.png");
    resetSS.textures[(int)ButtonVisual::Pressed]             = LoadTexture("assets/Texture/resetButtonPressed.png");

    auto& settingsSS = reg.emplace<SpriteStates>(settings);
    settingsSS.width = 50;
    settingsSS.height = 50;
    settingsSS.textures[0]                                   = LoadTexture("assets/Texture/helpButton.png");
    settingsSS.textures[(int)ButtonVisual::Pressed]          = LoadTexture("assets/Texture/helpButtonPressed.png");

    // 文字
    f.createText(140, 70, "STEP:", 26, LIGHTGRAY, 3);
    f.createTextWithTag<StepCounterTag>(240, 70, std::to_string(cache.stepCount), 26, LIGHTGRAY, 3);
    f.createText(140, 100, "TIME:", 26, LIGHTGRAY, 3);
    f.createTextWithTag<TimerDisplayTag>(240, 100, "--", 26, LIGHTGRAY, 3);
    
    f.createText(140, 160, "BESTS", 26, LIGHTGRAY, 3);
    f.createText(140, 190, "Step:", 26, YELLOW, 3);
    f.createTextWithTag<BestCounterTag>(240, 190, 
        (cache.bestRecords[cache.diskCount] == 0) ?
        "--" :
        std::to_string(cache.bestRecords[cache.diskCount]), 26, YELLOW, 3);
    f.createText(140, 220, "Time:", 26, YELLOW, 3);
    f.createTextWithTag<BestTimeTag>(240, 220, 
        (cache.bestTimes.count(cache.diskCount) && cache.bestTimes[cache.diskCount] > 0.0f) 
        ? TextFormat("%.1fs", cache.bestTimes[cache.diskCount]) : "--", 
        26, YELLOW, 3);
            
    f.createTextWithTag<DiskCountTag>(125, 642, std::to_string(cache.diskCount), 36, BLUE, 3);

    
    // 设置面板
    const float panelX = 2560.0f;
    const float panelY = 360.0f;

    auto settingsP = f.createPanel(panelX, panelY, 1280.0f, 720.0f, 4, PanelType::Settings);
    auto settingsX = f.createInstantButton(1280 + 260, 140, 20, 20, 5);
    reg.emplace<PanelCloseTag>(settingsX);

    // 指示灯
    auto diskInd = f.createIndicator(135, 625, 3);
    reg.emplace<DiskCountIndicatorTag>(diskInd);

    auto autoInd = f.createIndicator(290, 625, 3);
    reg.emplace<AutoDemoIndicatorTag>(autoInd);


    // 遮罩
    //f.createOverlay(-640.0f, 360.0f, 1280.0f, 720.0f, 6);
    //f.createOverlay(640.0f, 360.0f, 1280.0f, 720.0f, 6);


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

    auto helpPlainText1 = f.createText(panelX - 130, panelY + 45, " -   change disk number", 30, WHITE, 5);
    reg.emplace<SettingsPanelChildTag>(helpPlainText1);
    reg.emplace<SettingsPanelChildTag>(helpPlainText1);

    auto helpPlainText2 = f.createText(panelX - 130, panelY + 125, " -   auto demo", 30, WHITE, 5);
    reg.emplace<SettingsPanelChildTag>(helpPlainText2);
    reg.emplace<SettingsPanelChildTag>(helpPlainText2);

    auto helpPlainText3 = f.createText(panelX - 130, panelY + 205, " -   reset", 30, WHITE, 5);
    reg.emplace<SettingsPanelChildTag>(helpPlainText3);
    reg.emplace<SettingsPanelChildTag>(helpPlainText3);

    // X按钮
    auto exit = f.createInstantButton(panelX + 580, panelY + 300, 50, 50, 5);

    auto& exitSS = reg.emplace<SpriteStates>(exit);
    exitSS.width = 50;
    exitSS.height = 50;
    exitSS.textures[0]                              = LoadTexture("assets/Texture/cancelButton.png");
    exitSS.textures[(int)ButtonVisual::Pressed]     = LoadTexture("assets/Texture/cancelButtonPressed.png");

    reg.emplace<PanelCloseTag>(exit);
    reg.emplace<SettingsPanelChildTag>(exit);

}