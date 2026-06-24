#include "GamePhaseSystem.h"

#include <climits>

#include "Core/Resource.h"

#include "Component/SessionState.h"
#include "Component/BestRecord.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"

#include "Event/BestRecordChangedEvent.h"
#include "Event/RecordBrokenEvent.h"

std::string_view GamePhaseSystem::name() const { 
    return "gamePhase"; 
}

Phase GamePhaseSystem::phase() const { 
    return Phase::FixedUpdate; 
}

void GamePhaseSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 找 SessionState — 空则退
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sessionEntity = *sessionView.begin();
    auto& session = reg.get<SessionState>(sessionEntity);

    // 已完成或自动演示 — 跳过
    if (session.completed) return;

    // 找三根柱子的 NeedleIndex — 右柱 index=2
    entt::entity rightNeedle = entt::null;
    auto needleView = reg.view<const NeedleStack, const NeedleIndex>();
    for (auto [entity, stack, idx] : needleView.each()) {
        if (idx.index == 2) {
            rightNeedle = entity;
            break;
        }
    }
    if (rightNeedle == entt::null) return;

    auto& rightStack = reg.get<NeedleStack>(rightNeedle);
    if ((int)rightStack.disks.size() != session.diskCount) return;

    // 完成
    session.completed = true;
    if (session.isAutoDemo) return;   // 自动演示不更新最佳记录


    // 比较最佳纪录
    auto bestView = reg.view<BestRecord>();
    if (bestView.begin() == bestView.end()) return;
    auto bestEntity = *bestView.begin();
    auto& best = reg.get<BestRecord>(bestEntity);

    auto it = best.record.find(session.diskCount);
    int stepBest = (it != best.record.end()) ? it->second : INT_MAX;

    bool changed = false;

    if (session.stepCount < stepBest) {
        // 新步数纪录 → 同时覆盖步数和时间
        best.record[session.diskCount] = session.stepCount;
        if (session.timerRunning) {
            best.bestTimes[session.diskCount] = session.elapsedTime;
        }
        changed = true;
        res.events.trigger(RecordBrokenEvent{RecordType::Step});
    } else if (session.stepCount == stepBest && session.timerRunning) {
        // 步数持平 → 比较时间
        auto timeIt = best.bestTimes.find(session.diskCount);
        if (timeIt == best.bestTimes.end() || session.elapsedTime < timeIt->second) {
            best.bestTimes[session.diskCount] = session.elapsedTime;
            changed = true;
            res.events.trigger(RecordBrokenEvent{RecordType::Time});
        }
    }

    if (changed) {
        res.events.trigger(BestRecordChangedEvent{});
    }


}