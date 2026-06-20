#include "RectRenderSystem.h"

#include <vector>
#include <algorithm>

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/RectVisual.h"
#include "Component/Layer.h"
#include "Component/Interpolated.h"

std::string_view RectRenderSystem::name() const { return "rectRender"; }
Phase RectRenderSystem::phase() const { return Phase::Render; }

void RectRenderSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 1. 收集
    struct Item {
        int layer;
        Rectangle dst;
        Color color;
    };
    std::vector<Item> items;

    auto view = reg.view<const Position, const RectVisual>();
    for (auto [entity, pos, vis] : view.each()) {
        float drawX = pos.x;
        float drawY = pos.y;

        // 插值
        if (auto* interp = reg.try_get<const Interpolated>(entity)) {
            drawX = interp->prevX * (1.0f - res.alpha) + pos.x * res.alpha;
            drawY = interp->prevY * (1.0f - res.alpha) + pos.y * res.alpha;
        }

        int layer = 0;
        if (auto* l = reg.try_get<const Layer>(entity)) {
            layer = l->value;
        }

        items.push_back({
            layer,
            {drawX - vis.width / 2.0f, drawY - vis.height / 2.0f, vis.width, vis.height},
            vis.color
        });
    }

    // 2. 排序
    std::sort(items.begin(), items.end(),
        [](const Item& a, const Item& b) { return a.layer < b.layer; });

    // 3. 绘制
    for (auto& item : items) {
        DrawRectangleV({item.dst.x, item.dst.y}, {item.dst.width, item.dst.height}, item.color);
    }
}