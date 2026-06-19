#include "IndicatorSystem.h"

#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/RectVisual.h"
#include "Component/Tags.h"

std::string_view IndicatorSystem::name() const { return "indicator"; }
Phase IndicatorSystem::phase() const { return Phase::Logic; }

void IndicatorSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 找 SessionState 实体
    auto sessionView = reg.view<const SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sessionEntity = *sessionView.begin();

    bool diskChanged = reg.all_of<DiskCountChangedTag>(sessionEntity);
    bool autoDemo    = reg.all_of<NextAutoDemoTag>(sessionEntity);

    // 数字指示灯
    auto diskIndicators = reg.view<RectVisual, DiskCountIndicatorTag>();
    for (auto [entity, vis] : diskIndicators.each()) {
        vis.color = diskChanged ? GREEN : DARKGRAY;
    }

    // 自动演示指示灯
    auto autoIndicators = reg.view<RectVisual, AutoDemoIndicatorTag>();
    for (auto [entity, vis] : autoIndicators.each()) {
        vis.color = autoDemo ? GREEN : DARKGRAY;
    }
}