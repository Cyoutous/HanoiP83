#include "EntityFactory.h"

#include "Core/Resource.h"

#include "Component/Position.h"
#include "Component/RectVisual.h"
#include "Component/SpriteStates.h"
#include "Component/Layer.h"
#include "Component/Clickable.h"
#include "Component/NeedleState.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"
#include "Component/DiskData.h"
#include "Component/Interpolated.h"
#include "Component/SessionState.h"
#include "Component/NextSessionConfig.h"
#include "Component/BestRecord.h"

#include "Component/ButtonState.h"
#include "Component/ToggleState.h"
#include "Component/IndicatorState.h"
#include "Component/TextLabel.h"
#include "Component/Panel.h"

#include "Component/Setting.h"
#include "Component/PendingSetting.h"
#include "Component/HistoryEntry.h"

#include "Component/Tags.h" 

EntityFactory::EntityFactory(entt::registry& reg, Resource& res) 
    : reg(reg), res(res) {}


// 柱子
entt::entity EntityFactory::createNeedle(float x, float y, int index) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<Layer>(entity, 1);
    //reg.emplace<RectVisual>(entity, 16.0f, res.needleHeight, GRAY);   // 柱宽16, 高280
    auto& ss = reg.emplace<SpriteStates>(entity);
    ss.width = 16;
    ss.height = 280;
    ss.textures[0]                              = LoadTexture("assets/Texture/Needle1.png");
    ss.textures[(int)NeedleVisual::Selected]    = LoadTexture("assets/Texture/Needle2.png");
    ss.textures[(int)NeedleVisual::InvalidTarget] = LoadTexture("assets/Texture/Needle3.png");

    float clickW = 280.0f;
    float clickH = res.needleHeight;
    reg.emplace<Clickable>(entity, Vector2{-clickW / 2, -clickH / 2}, clickW, clickH);

    reg.emplace<NeedleState>(entity);
    reg.emplace<NeedleStack>(entity);
    reg.emplace<NeedleIndex>(entity, index);
    return entity;
}

// 盘子
entt::entity EntityFactory::createDisk(float x, float y, int diskIndex, int totalDisks) {
    auto entity = reg.create();
    float w = diskWidth(diskIndex, totalDisks);
    float h = res.diskHeight;
    reg.emplace<Position>(entity, x, y);
    reg.emplace<Layer>(entity, 2);
    //reg.emplace<RectVisual>(entity, w, h, ColorFromHSV(diskIndex * 50.0f, 0.7f, 0.9f));
    auto& ss = reg.emplace<SpriteStates>(entity);
    ss.width = w;
    ss.height = h;
    ss.textures[0] = LoadTexture(TextFormat("assets/Texture/Disk%d.png", diskIndex));
    
    reg.emplace<DiskData>(entity, diskIndex);
    reg.emplace<Interpolated>(entity, x, y);          // 固定步长插值
    return entity;
}
float EntityFactory::diskWidth(int diskIndex, int totalDisks) const {
    float t = totalDisks > 1 ? (float)diskIndex / (totalDisks - 1) : 0.0f; // 0 = 最小, 1 = 最大
    return res.minDiskWidth + (res.maxDiskWidth - res.minDiskWidth) * (1.0f - t);
}

// 关卡状态
entt::entity EntityFactory::createSessionState(int diskCount) {
    auto entity = reg.create();
    reg.emplace<SessionState>(entity, diskCount, 0, false, false);
    return entity;
}

entt::entity EntityFactory::createNextSessionConfig(int diskCount) {
    auto entity = reg.create();
    reg.emplace<NextSessionConfig>(entity, diskCount, false);
    return entity;
}

// 最佳记录
entt::entity EntityFactory::createBestRecord() {
    auto entity = reg.create();
    reg.emplace<BestRecord>(entity);
    return entity;
}

// 背景实体
entt::entity EntityFactory::createBackground() {
    auto entity = reg.create();
    reg.emplace<Position>(entity, 640.0f, 360.0f);        // 逻辑屏幕中心
    reg.emplace<Layer>(entity, 0);
    //reg.emplace<RectVisual>(entity, 1280.0f, 720.0f, Color{30, 30, 30, 255});
    auto& ss = reg.emplace<SpriteStates>(entity);
    ss.width = 1280.0f;
    ss.height = 720.0f;
    ss.textures[0] = LoadTexture("assets/Texture/background.png");

    return entity;
}

// ------

// 即时响应按钮
entt::entity EntityFactory::createInstantButton(float x, float y, float w, float h, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    //reg.emplace<RectVisual>(entity, w, h, DARKGRAY);
    reg.emplace<Layer>(entity, layer);
    reg.emplace<Clickable>(entity, Vector2{-w / 2, -h / 2}, w, h);
    reg.emplace<ButtonState>(entity);
    // 不挂 ToggleState，ButtonSystem 据此判断为即时按钮
    return entity;
}

// 二态按钮
entt::entity EntityFactory::createToggleButton(float x, float y, float w, float h, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    //reg.emplace<RectVisual>(entity, w, h, DARKGRAY);
    reg.emplace<Layer>(entity, layer);
    reg.emplace<Clickable>(entity, Vector2{-w / 2, -h / 2}, w, h);
    reg.emplace<ToggleState>(entity);
    return entity;
}

// 文字
entt::entity EntityFactory::createText(float x, float y, const std::string& text,
                                            int fontSize, Color color, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<TextLabel>(entity, text, fontSize, color);
    reg.emplace<Layer>(entity, layer);
    return entity;
}

// 浮窗
entt::entity EntityFactory::createPanel(float x, float y, float w, float h,
                                        int layer, PanelType type) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<Layer>(entity, layer);
    //reg.emplace<RectVisual>(entity, w, h, Color{40, 40, 40, 255});
    auto& ss = reg.emplace<SpriteStates>(entity);
    ss.width = 1280.0f;
    ss.height = 720.0f;
    ss.textures[0] = LoadTexture("assets/Texture/cover2.png");

    reg.emplace<Clickable>(entity, Vector2{-w / 2, -h / 2}, w, h);
    reg.emplace<Panel>(entity, type, false);
    reg.emplace<Interpolated>(entity);    // 滑入滑出需要插值
    return entity;
}

// 遮罩
entt::entity EntityFactory::createOverlay(float x, float y, float w, float h, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<Layer>(entity, layer);
    reg.emplace<RectVisual>(entity, w, h, Fade(BLACK, 0.5f));

    reg.emplace<Interpolated>(entity);
    return entity;
}

//指示灯
entt::entity EntityFactory::createIndicator(float x, float y, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<Layer>(entity, layer);
    //reg.emplace<RectVisual>(entity, 15.0f, 15.0f, DARKGRAY);
    auto& ss = reg.emplace<SpriteStates>(entity);
    ss.width = 15; ss.height = 15;
    ss.textures[0] = LoadTexture("assets/Texture/indicatorOff.png");
    ss.textures[1] = LoadTexture("assets/Texture/indicatorOn.png");

    reg.emplace<IndicatorState>(entity);

    return entity;
}

// ---------

// 设置
entt::entity EntityFactory::createSettings() {
    auto entity = reg.create();
    reg.emplace<Settings>(entity);
    return entity;
}

// 历史容器
entt::entity EntityFactory::createHistoryEntry(int diskCount, int steps,
                                                bool completed, const std::string& timestamp) {
    auto entity = reg.create();
    reg.emplace<HistoryEntry>(entity, HistoryEntry{diskCount, steps, completed, timestamp});
    return entity;
}