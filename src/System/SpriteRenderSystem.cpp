#include "SpriteRenderSystem.h"

#include <vector>
#include <algorithm>

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/StaticSprite.h"
#include "Component/SpriteStates.h"
#include "Component/Layer.h"
#include "Component/Interpolated.h"
#include "Component/NeedleState.h"
#include "Component/ButtonState.h"
#include "Component/ToggleState.h"

std::string_view SpriteRenderSystem::name() const { return "spriteRender"; }
Phase SpriteRenderSystem::phase() const { return Phase::Render; }

void SpriteRenderSystem::onUpdate(entt::registry& reg, Resource& res) {
    struct Item {
        int layer;
        Texture2D texture;
        Rectangle src;
        Rectangle dst;
    };
    std::vector<Item> items;

    auto view = reg.view<Position, SpriteStates>();
    for (auto [entity, pos, ss] : view.each()) {
        float drawX = pos.x;
        float drawY = pos.y;

        if (auto* interp = reg.try_get<Interpolated>(entity)) {
            drawX = interp->prevX * (1.0f - res.alpha) + pos.x * res.alpha;
            drawY = interp->prevY * (1.0f - res.alpha) + pos.y * res.alpha;
        }

        int layer = 0;
        if (auto* l = reg.try_get<Layer>(entity)) {
            layer = l->value;
        }

        // 默认贴图
        int key = 0;

        // 根据状态 Component 选 key
        if (auto* state = reg.try_get<NeedleState>(entity)) {
            key = (int)state->visual;
        } else if (auto* btn = reg.try_get<ButtonState>(entity)) {
            key = (int)btn->visual;
        } else if (auto* tog = reg.try_get<ToggleState>(entity)) {
            key = (int)tog->visual;
        }

        // 取值，key 不存在则用 0 的默认贴图
        Texture2D tex = ss.textures.count(key) ? ss.textures[key] : ss.textures[0];
        if (tex.id == 0) continue;  // 没加载过的跳过

        items.push_back({
            layer, tex,
            {0, 0, ss.width, ss.height},
            {drawX - ss.width / 2.0f, drawY - ss.height / 2.0f, ss.width, ss.height}
        });
    }

    std::sort(items.begin(), items.end(),
        [](const Item& a, const Item& b) { return a.layer < b.layer; });

    for (auto& item : items) {
        DrawTexturePro(item.texture, item.src, item.dst, {0, 0}, 0.0f, WHITE);
    }
}