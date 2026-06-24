#include "TimerSystem.h"
#include <unordered_map>
#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Event/SessionStateChangedEvent.h"
#include "Event/TimerStateChangedEvent.h"

std::string_view TimerSystem::name() const { 
    return "timerSystem"; 
}

Phase TimerSystem::phase() const { 
    return Phase::FixedUpdate; 
}

void TimerSystem::onUpdate(entt::registry& reg, Resource& res) {
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto& session = reg.get<SessionState>(*sessionView.begin());

    if (session.completed && session.timerRunning) {
        session.timerRunning = false;
        res.events.trigger(SessionStateChangedEvent{});
        return;
    }

    if (!session.timerRunning || session.timeUp) return;

    session.elapsedTime += res.fixedDt;

    auto it = res.timeLimits.find(session.diskCount);
    float limit = (it != res.timeLimits.end()) ? it->second : 9999.0f;


    if (session.elapsedTime >= limit) {
        session.timeUp = true;
        session.timerRunning = false;
        res.events.trigger(TimerStateChangedEvent{}); 
    }

    res.events.trigger(SessionStateChangedEvent{});
}