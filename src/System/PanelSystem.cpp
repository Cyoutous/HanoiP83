#include "PanelSystem.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/Panel.h"
#include "Component/TweenPosition.h"
#include "Component/TweenSequence.h"
#include "Component/Tags.h"
#include "Event/ButtonClickedEvent.h"

std::string_view PanelSystem::name() const { 
    return "panelSystem"; 
}

Phase PanelSystem::phase() const { 
    return Phase::Logic; 
}

// ---------- 辅助 ----------

entt::entity PanelSystem::findOpenPanel() {
    auto& reg = *_reg;
    auto view = reg.view<const Panel>();
    for (auto [entity, panel] : view.each()) {
        if (panel.isOpen) return entity;
    }
    return entt::null;
}

entt::entity PanelSystem::findPanelByType(PanelType type) {
    auto& reg = *_reg;
    auto view = reg.view<const Panel>();
    for (auto [entity, panel] : view.each()) {
        if (panel.type == type) return entity;
    }
    return entt::null;
}

void PanelSystem::openPanel(entt::entity panel) {
    auto& reg = *_reg;
    auto& p = reg.get<Panel>(panel);
    if (p.isOpen) return;

    p.isOpen = true;
    reg.remove<TweenPosition, TweenSequence>(panel);
    auto& pos = reg.get<Position>(panel);
    reg.emplace<TweenPosition>(panel, pos.x, pos.y, 640.0f, pos.y, 0.3f, 0.0f, EaseMode::EaseOut);
}

void PanelSystem::closePanel(entt::entity panel) {
    auto& reg = *_reg;
    auto& p = reg.get<Panel>(panel);
    if (!p.isOpen) return;

    p.isOpen = false;
    reg.remove<TweenPosition, TweenSequence>(panel);
    auto& pos = reg.get<Position>(panel);
    reg.emplace<TweenPosition>(panel, pos.x, pos.y, 1280.0f, pos.y, 0.3f, 0.0f, EaseMode::EaseIn);
}

// ---------- 事件处理 ----------

void PanelSystem::onButtonClicked(ButtonClickedEvent& event) {
    auto& reg = *_reg;

    // X 按钮 → 关当前面板
    if (reg.all_of<PanelCloseTag>(event.button)) {
        auto open = findOpenPanel();
        if (open != entt::null) closePanel(open);
        return;
    }

    // 已有一个面板开着 → 先关
    auto open = findOpenPanel();

    // 历史面板按钮
    if (reg.all_of<HistoryPanelToggleTag>(event.button)) {
        if (open != entt::null) closePanel(open);
        auto panel = findPanelByType(PanelType::History);
        if (panel != entt::null) openPanel(panel);
        return;
    }

    // 设置面板按钮
    if (reg.all_of<SettingsPanelToggleTag>(event.button)) {
        if (open != entt::null) closePanel(open);
        auto panel = findPanelByType(PanelType::Settings);
        if (panel != entt::null) openPanel(panel);
        return;
    }
}

// ---------- 生命周期 ----------

void PanelSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    res.events.sink<ButtonClickedEvent>()
        .connect<&PanelSystem::onButtonClicked>(*this);
}

void PanelSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 面板的滑动由 TweenSystem 处理，本 System 无每帧逻辑
}