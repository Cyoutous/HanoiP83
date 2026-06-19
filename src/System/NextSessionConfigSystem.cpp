#include "NextSessionConfigSystem.h"
#include <algorithm>
#include "Core/Resource.h"
#include "Component/NextSessionConfig.h"
#include "Component/Tags.h"
#include "Event/ButtonClickedEvent.h"
#include "Event/ToggleChangedEvent.h"

std::string_view NextSessionConfigSystem::name() const { return "nextSessionConfig"; }
Phase NextSessionConfigSystem::phase() const { return Phase::Logic; }

void NextSessionConfigSystem::setDiskCount(entt::registry& reg, int count) {
    auto view = reg.view<NextSessionConfig>();
    if (view.begin() == view.end()) return;
    auto& next = reg.get<NextSessionConfig>(*view.begin());
    next.diskCount = std::clamp(count, 3, 10);
}

void NextSessionConfigSystem::onButtonClicked(ButtonClickedEvent& event) {
    auto& reg = *_reg;
    auto view = reg.view<NextSessionConfig>();
    if (view.begin() == view.end()) return;
    auto& next = reg.get<NextSessionConfig>(*view.begin());

    if (reg.all_of<DiskCountArrowUpTag>(event.button)) {
        setDiskCount(reg, next.diskCount + 1);
    }
    if (reg.all_of<DiskCountArrowDownTag>(event.button)) {
        setDiskCount(reg, next.diskCount - 1);
    }
}

void NextSessionConfigSystem::onToggleChanged(ToggleChangedEvent& event) {
    auto& reg = *_reg;
    if (!reg.all_of<AutoDemoButtonTag>(event.button)) return;

    auto view = reg.view<NextSessionConfig>();
    if (view.begin() == view.end()) return;
    auto& next = reg.get<NextSessionConfig>(*view.begin());
    next.autoDemo = !next.autoDemo;
}

void NextSessionConfigSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    res.events.sink<ButtonClickedEvent>()
        .connect<&NextSessionConfigSystem::onButtonClicked>(*this);
    res.events.sink<ToggleChangedEvent>()
        .connect<&NextSessionConfigSystem::onToggleChanged>(*this);
}

void NextSessionConfigSystem::onUpdate(entt::registry& reg, Resource& res) {}