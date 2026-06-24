#include "SessionStateUpdateSystem.h"

#include "raylib.h"

#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/NextSessionConfig.h"
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
    auto nextView = reg.view<const NextSessionConfig>();
    if (nextView.begin() != nextView.end()) {
        int nextDisk = reg.get<const NextSessionConfig>(*nextView.begin()).diskCount;
        for (auto [entity, label] : diskView.each()) {
            label.text = std::to_string(nextDisk);
        }
    }
    

    // 计时器
    auto timerView = reg.view<TextLabel, TimerDisplayTag>();
    for (auto [entity, label] : timerView.each()) {
        if (session.timeUp) {
            label.text = "TIME UP";
            label.color = RED;
        } else if (session.completed) {
            label.color = GREEN;
        } else if (session.timerRunning) {
            auto it = _res->timeLimits.find(session.diskCount);
            float limit = (it != _res->timeLimits.end()) ? it->second : 9999.0f;
            int remaining = (int)(limit - session.elapsedTime);
            label.text = std::to_string(remaining) + "s";
            label.color = WHITE;
        } else {
            label.text = "--";
            label.color = LIGHTGRAY;
        }
    }

    //刷新最佳记录标签
    auto bestView = reg.view<const BestRecord>();
    if (bestView.begin() != bestView.end()) {
        auto& best = reg.get<const BestRecord>(*bestView.begin());

        auto it = best.record.find(session.diskCount);
        std::string stepDisplay = (it != best.record.end() && it->second > 0)
            ? std::to_string(it->second) : "--";
        auto stepView = reg.view<TextLabel, BestCounterTag>();
        for (auto [entity, label] : stepView.each()) {
            label.text = stepDisplay;
        }

        auto timeIt = best.bestTimes.find(session.diskCount);
        std::string timeDisplay = (timeIt != best.bestTimes.end() && timeIt->second > 0.0f)
            ? TextFormat("%.1fs", timeIt->second) : "--";
        auto timeView = reg.view<TextLabel, BestTimeTag>();
        for (auto [entity, label] : timeView.each()) {
            label.text = timeDisplay;
        }
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
    std::string display = (it != best.record.end() && it->second > 0)
        ? std::to_string(it->second) : "--";

    auto view = reg.view<TextLabel, BestCounterTag>();
    for (auto [entity, label] : view.each()) {
        label.text = display;
    }

    auto timeView = reg.view<TextLabel, BestTimeTag>();
    auto timeIt = best.bestTimes.find(session.diskCount);
    std::string timeDisplay = (timeIt != best.bestTimes.end() && timeIt->second > 0.0f)
        ? TextFormat("%.1fs", timeIt->second) : "--";
    for (auto [entity, label] : timeView.each()) {
        label.text = timeDisplay;
    }
}

void SessionStateUpdateSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    _res = &res;

    res.events.sink<SessionStateChangedEvent>()
        .connect<&SessionStateUpdateSystem::onSessionStateChanged>(*this);

    res.events.sink<BestRecordChangedEvent>()
        .connect<&SessionStateUpdateSystem::onBestRecordChanged>(*this);
}


void SessionStateUpdateSystem::onUpdate(entt::registry& reg, Resource& res) {

}