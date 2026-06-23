#include "RenderSystem.h"

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Layer.h"
#include "Component/Position.h"
#include "Component/Interpolated.h"
#include "Component/SpriteStates.h"
#include "Component/TextLabel.h"

#include "Component/NeedleState.h"
#include "Component/ButtonState.h"
#include "Component/ToggleState.h"

std::string_view RenderSystem::name() const { 
    return "render"; 
}

Phase RenderSystem::phase() const { 
    return Phase::Render; 
}

void RenderSystem::onUpdate(entt::registry& reg, Resource& res) {
    struct Item {
        int layer;
        enum Type { Sprite, Text } type;
        Texture2D texture;
        Rectangle src, dst;
        std::string text;
        int fontSize;
        Color color;
    };
    std::vector<Item> items;

    auto view = reg.view<Layer>();
    for (auto entity : view) {
        int layer = reg.get<Layer>(entity).value;

        if (!reg.all_of<Position>(entity)) continue;
        auto& pos = reg.get<Position>(entity);
        float drawX = pos.x, drawY = pos.y;
        if (auto* interp = reg.try_get<const Interpolated>(entity)) {
            drawX = interp->prevX * (1.0f - res.alpha) + pos.x * res.alpha;
            drawY = interp->prevY * (1.0f - res.alpha) + pos.y * res.alpha;
        }

        if (auto* ss = reg.try_get<SpriteStates>(entity)) {

            int key = 0;
            // 根据状态 Component 选 key
            if (auto* state = reg.try_get<NeedleState>(entity)) {
                key = (int)state->visual;
            } else if (auto* btn = reg.try_get<ButtonState>(entity)) {
                key = (int)btn->visual;
            } else if (auto* tog = reg.try_get<ToggleState>(entity)) {
                key = (int)tog->visual;
            }

            Texture2D tex = ss->textures.count(key) ? ss->textures[key] : ss->textures[0];
            if (tex.id == 0) continue;
            items.push_back({layer, Item::Sprite, tex,
                {0, 0, ss->width, ss->height},
                {drawX - ss->width/2, drawY - ss->height/2, ss->width, ss->height}});
        }
        else if (auto* label = reg.try_get<TextLabel>(entity)) {

            items.push_back({layer, Item::Text, {}, {}, 
                {drawX, drawY, 0, 0},
                label->text, label->fontSize, label->color});
        }
    }

    std::sort(items.begin(), items.end(),
        [](auto& a, auto& b) { return a.layer < b.layer; });

    for (auto& item : items) {
        if (item.type == Item::Sprite)
            DrawTexturePro(item.texture, item.src, item.dst, {0,0}, 0, WHITE);
        else
            DrawTextEx(GetFontDefault(), item.text.c_str(), {item.dst.x, item.dst.y},
                (float)item.fontSize, 1, item.color);
    }
}
