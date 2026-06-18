#include "SessionStateUpdateSystem.h"

#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/BestRecord.h"
#include "Component/TextLabel.h"
#include "Component/Tags.h"

#include "Event/SessionStateChangedEvent.h"
#include "Event/BestRecordChangedEvent.h"

std::string_view SessionStateUpdateSystem::name() const { return "sessionStateUpdateSystem"; }

Phase SessionStateUpdateSystem::phase() const { return Phase::Logic; }

void SessionStateUpdateSystem::onSessionStateChanged(SessionStateChangedEvent&) {
    auto& reg = *_reg;

    auto sessionView = reg.view<const SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sEntity = *sessionView.begin();
    const auto& session = reg.get<const SessionState>(sEntity);

    auto stepView = reg.view<TextLabel, StepCounterTag>();
    for (auto [entity, label] : stepView.each()) {
        label.text = std::to_string(session.stepCount);
    }

    auto diskView = reg.view<TextLabel, DiskCountTag>();
    for (auto [entity, label] : diskView.each()) {
        label.text = std::to_string(session.diskCount);
    }
}

void SessionStateUpdateSystem::onBestRecordChanged(BestRecordChangedEvent&) {
    auto& reg = *_reg;

    auto bestView = reg.view<const BestRecord>();
    if (bestView.begin() == bestView.end()) return;
    auto bEntity = *bestView.begin();
    const auto& best = reg.get<const BestRecord>(bEntity);

    auto sessionView = reg.view<const SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sEntity = *sessionView.begin();
    const auto& session = reg.get<const SessionState>(sEntity);

    auto it = best.record.find(session.diskCount);
    std::string display = (it != best.record.end())
        ? std::to_string(it->second) : "--";

    auto view = reg.view<TextLabel, BestCounterTag>();
    for (auto [entity, label] : view.each()) {
        label.text = display;
    }
}

void SessionStateUpdateSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;

    res.events.sink<SessionStateChangedEvent>()
        .connect<&SessionStateUpdateSystem::onSessionStateChanged>(*this);

    res.events.sink<BestRecordChangedEvent>()
        .connect<&SessionStateUpdateSystem::onBestRecordChanged>(*this);
}


void SessionStateUpdateSystem::onUpdate(entt::registry& reg, Resource& res) {

}