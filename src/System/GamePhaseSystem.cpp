#include "GamePhaseSystem.h"

#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/BestRecord.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"
#include "Event/BestRecordChangedEvent.h"

std::string_view GamePhaseSystem::name() const { return "gamePhase"; }
Phase GamePhaseSystem::phase() const { return Phase::FixedUpdate; }

void GamePhaseSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 找 SessionState — 空则退
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sessionEntity = *sessionView.begin();
    auto& session = reg.get<SessionState>(sessionEntity);

    // 已完成或自动演示 — 跳过
    if (session.completed || session.isAutoDemo) return;

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

    // 比较最佳纪录
    auto bestView = reg.view<BestRecord>();
    if (bestView.begin() == bestView.end()) return;
    auto bestEntity = *bestView.begin();
    auto& best = reg.get<BestRecord>(bestEntity);

    auto it = best.record.find(session.diskCount);
    bool newRecord = (it == best.record.end() || session.stepCount < it->second);
    bool matched   = (it != best.record.end() && session.stepCount == it->second);

    if (newRecord) {
        best.record[session.diskCount] = session.stepCount;
    }

    if (newRecord || matched) {
        res.events.trigger<BestRecordChangedEvent>();
    }

}