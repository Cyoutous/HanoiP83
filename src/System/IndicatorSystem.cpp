#include "IndicatorSystem.h"
#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/NextSessionConfig.h"
#include "Component/RectVisual.h"
#include "Component/Tags.h"

std::string_view IndicatorSystem::name() const { return "indicator"; }
Phase IndicatorSystem::phase() const { return Phase::Logic; }

void IndicatorSystem::onUpdate(entt::registry& reg, Resource& res) {
    auto sessionView = reg.view<const SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto& session = reg.get<const SessionState>(*sessionView.begin());

    auto nextView = reg.view<const NextSessionConfig>();
    if (nextView.begin() == nextView.end()) return;
    auto& next = reg.get<const NextSessionConfig>(*nextView.begin());

    bool diskChanged = (next.diskCount != session.diskCount);
    bool autoDemo    = next.autoDemo;

    auto diskIndicators = reg.view<RectVisual, DiskCountIndicatorTag>();
    for (auto [entity, vis] : diskIndicators.each()) {
        vis.color = diskChanged ? GREEN : DARKGRAY;
    }

    auto autoIndicators = reg.view<RectVisual, AutoDemoIndicatorTag>();
    for (auto [entity, vis] : autoIndicators.each()) {
        vis.color = autoDemo ? GREEN : DARKGRAY;
    }
}