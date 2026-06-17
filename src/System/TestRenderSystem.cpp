#include "TestRenderSystem.h"

#include <entt/entt.hpp>
#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/StaticSprite.h"

std::string_view TestRenderSystem::name() const {
    return "TestRenderSystem";
}

Phase TestRenderSystem::phase() const {
    return Phase::Render;
}

void TestRenderSystem::onUpdate(entt::registry& reg, Resource& res) {
    
    auto view = reg.view<const Position, const StaticSprite>();
    for (auto [entity, pos, sprite] : view.each()) {
        DrawTexturePro(
            sprite.texture, 
            {0, 0, sprite.width, sprite.height},
            {pos.x, pos.y, sprite.width, sprite.height},
            {0, 0}, 0, WHITE
        );
    }
}