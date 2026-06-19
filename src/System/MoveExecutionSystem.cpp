#include "MoveExecutionSystem.h"
#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/NeedleStack.h"
//#include "Component/NeedleIndex.h"
#include "Component/DiskData.h"
#include "Component/SessionState.h"
#include "Component/TweenPosition.h"
#include "Component/TweenSequence.h"
#include "Type/MoveAction.h"
#include "Event/SessionStateChangedEvent.h"

std::string_view MoveExecutionSystem::name() const { return "moveExecution"; }
Phase MoveExecutionSystem::phase() const { return Phase::FixedUpdate; }

// ---------- 辅助 ----------

bool MoveExecutionSystem::hasActiveTween(entt::registry& reg) const {
    auto view = reg.view<const DiskData>();
    for (auto entity : view) {   // 不加 .each()，直接遍历 entity
        if (reg.all_of<TweenPosition>(entity) || reg.all_of<TweenSequence>(entity))
            return true;
    }
    return false;
}

float MoveExecutionSystem::calcDiskY(entt::registry& reg, entt::entity needle, int stackIndex) const {
    auto& needlePos = reg.get<Position>(needle);
    float baseY = needlePos.y + 129.0f;
    return baseY - (stackIndex - 1) * 22.0f;
}

// ---------- 主体 ----------

void MoveExecutionSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 动画还在跑 → 不取下一个
    if (hasActiveTween(reg)) return;

    // 队列空
    if (res.actionQueue.empty()) return;

    auto action = res.actionQueue.front();
    res.actionQueue.pop();

    // 找 SessionState 实体
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto sessionEntity = *sessionView.begin();
    auto& session = reg.get<SessionState>(sessionEntity);

    // 改栈
    auto& fromStack = reg.get<NeedleStack>(action.fromPillar);
    auto& toStack   = reg.get<NeedleStack>(action.toPillar);
    fromStack.disks.pop_back();
    toStack.disks.push_back(action.disk);

    // 步数 +1
    session.stepCount++;
    res.events.trigger<SessionStateChangedEvent>();

    // 堆叠索引 = toStack 当前大小
    int stackIndex = (int)toStack.disks.size();
    float targetX = reg.get<Position>(action.toPillar).x;
    float targetY = calcDiskY(reg, action.toPillar, stackIndex);

    // 清旧动画，挂三段 TweenSequence
    reg.remove<TweenPosition, TweenSequence>(action.disk);
    auto& diskPos = reg.get<Position>(action.disk);
    auto& seq = reg.emplace<TweenSequence>(action.disk);
    seq.steps.push_back({diskPos.x, diskPos.y - 20.0f,      0.08f, EaseMode::EaseOut, 0.0f});  // 浮起
    seq.steps.push_back({targetX,   diskPos.y - 20.0f,      0.0f,  EaseMode::Instant, 0.0f});  // 瞬移
    seq.steps.push_back({targetX,   targetY,                0.12f, EaseMode::EaseOut, 0.0f});  // 落下
}