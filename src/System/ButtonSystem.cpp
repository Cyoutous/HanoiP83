#include "ButtonSystem.h"

#include "Core/Resource.h"
#include "Component/ButtonState.h"
#include "Component/ToggleState.h"
#include "Component/InputResponse.h"
#include "Event/ButtonClickedEvent.h"
#include "Event/ToggleChangedEvent.h"
#include "Event/SettingsButtonEvent.h"

std::string_view ButtonSystem::name() const { 
    return "buttonSystem"; 
}

Phase ButtonSystem::phase() const { 
    return Phase::Logic; 
}

void ButtonSystem::onUpdate(entt::registry& reg, Resource& res) {
    // === 即时按钮（有 ButtonState，无 ToggleState）===
    auto instantView = reg.view<ButtonState>();
    for (auto [entity, btn] : instantView.each()) {
        auto* input = reg.try_get<const InputResponse>(entity);
        if (input && input->clicked) {
            btn.visual = ButtonVisual::Idle;
            res.events.trigger(ButtonClickedEvent{entity});
        } else if (input && input->pressed) {
            btn.visual = ButtonVisual::Pressed;
        } else {
            btn.visual = ButtonVisual::Idle;
        }
    }


    // === 二态按钮（有 ToggleState）===
    auto toggleView = reg.view<ToggleState, const InputResponse>();
    for (auto [entity, toggle, input] : toggleView.each()) {
        if (input.clicked) {
            toggle.visual = (toggle.visual == ToggleVisual::Off)
                ? ToggleVisual::On : ToggleVisual::Off;
            res.events.trigger(ToggleChangedEvent{entity});
        }
        // 不响应 pressed — 二态按钮视觉不由按压决定
    }
}