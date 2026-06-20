#include "UIRenderSystem.h"

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/TextLabel.h"

std::string_view UIRenderSystem::name() const { 
    return "uiRenderSystem"; 
}

Phase UIRenderSystem::phase() const { 
    return Phase::Render; 
}

void UIRenderSystem::onUpdate(entt::registry& reg, Resource& res) {
    auto view = reg.view<const Position, const TextLabel>();
    for (auto [entity, pos, label] : view.each()) {
        DrawTextEx(
            GetFontDefault(),
            label.text.c_str(),
            {pos.x, pos.y},
            (float)label.fontSize,
            1.0f,
            label.color
        );
    }
}