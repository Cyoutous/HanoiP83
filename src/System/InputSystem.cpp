#include "InputSystem.h"

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/Clickable.h"
#include "Component/Layer.h"
#include "Component/InputResponse.h"

std::string_view InputSystem::name() const {
    return "inputSystem"; 
}

Phase InputSystem::phase() const { 
    return Phase::Logic; 
}

void InputSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 1. 清空上一帧残余
    reg.clear<InputResponse>();

    // 2. 鼠标坐标
    Vector2 mouse = res.windowState.toLogic(GetMouseX(), GetMouseY());

    // 3. 找鼠标命中的最高层 Clickable
    entt::entity topEntity = entt::null;
    int topLayer = -1;
    auto view = reg.view<const Position, const Clickable, const Layer>();
    for (auto [entity, pos, clickable, layer] : view.each()) {
        Rectangle worldBounds = {
            pos.x + clickable.clickOffset.x,
            pos.y + clickable.clickOffset.y,
            clickable.width,
            clickable.height
        };
        if (CheckCollisionPointRec(mouse, worldBounds) && layer.value > topLayer) {
            topEntity = entity;
            topLayer = layer.value;
        }
    }

    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    // 4. 写入 InputResponse
    if (mousePressed && topEntity != entt::null) {
        reg.emplace_or_replace<InputResponse>(topEntity, false, true);
        _pressedEntity = topEntity;

    } else if (mouseDown && _pressedEntity != entt::null) {
        // 按住中，鼠标可能移到了另一个实体
        if (topEntity == _pressedEntity) {
            reg.emplace_or_replace<InputResponse>(topEntity, false, true);
        } else {
            // 移开了 — 旧实体失去 pressed
            if (topEntity != entt::null) {
                reg.emplace_or_replace<InputResponse>(topEntity, false, true);
                _pressedEntity = topEntity;
            }
        }

    } else if (mouseReleased && _pressedEntity != entt::null) {
        if (topEntity == _pressedEntity) {
            reg.emplace_or_replace<InputResponse>(topEntity, true, false);  // clicked
        }
        _pressedEntity = entt::null;

    } else if (!mouseDown) {
        _pressedEntity = entt::null;
    }
}