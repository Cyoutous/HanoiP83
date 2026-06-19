#include "AutoDemoSystem.h"
#include "Core/Resource.h"
#include "Component/SessionState.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"
#include "Component/DiskData.h"
#include "Component/Position.h"
#include "Component/TweenPosition.h"
#include "Component/TweenSequence.h"
#include "Type/MoveAction.h"

std::string_view AutoDemoSystem::name() const { return "autoDemo"; }
Phase AutoDemoSystem::phase() const { return Phase::FixedUpdate; }

// ---------- 辅助 ----------

void AutoDemoSystem::init(entt::registry& reg) {
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto& session = reg.get<SessionState>(*sessionView.begin());
    _totalDisks = session.diskCount;
    _moveIndex = 1;

    // 找最小盘
    auto diskView = reg.view<const DiskData>();
    int targetSize = _totalDisks - 1;
    for (auto [entity, data] : diskView.each()) {
        if (data.size == targetSize) {
            _smallest = entity;
            break;
        }
    }
}

int AutoDemoSystem::findNeedleIndex(entt::registry& reg, entt::entity disk) const {
    auto view = reg.view<const NeedleStack, const NeedleIndex>();
    for (auto [entity, stack, idx] : view.each()) {
        for (auto d : stack.disks) {
            if (d == disk) return idx.index;
        }
    }
    return -1;
}

entt::entity AutoDemoSystem::findNeedleByIndex(entt::registry& reg, int index) const {
    auto view = reg.view<const NeedleIndex>();
    for (auto [entity, idx] : view.each()) {
        if (idx.index == index) return entity;
    }
    return entt::null;
}

bool AutoDemoSystem::hasActiveAnimation(entt::registry& reg) const {
    auto view = reg.view<const DiskData>();
    for (auto entity : view) {   // 不加 .each()
        if (reg.all_of<TweenPosition>(entity) || reg.all_of<TweenSequence>(entity))
            return true;
    }
    return false;
}

int AutoDemoSystem::nextPeg(int fromIndex, int diskSize) const {
    // 顺时针序列，每个盘按其奇偶和 totalDisks 奇偶决定方向
    // diskSize 奇偶：
    //   totalDisks 偶：  奇数盘 0→1→2→0, 偶数盘 0→2→1→0
    //   totalDisks 奇：  奇数盘 0→2→1→0, 偶数盘 0→1→2→0
    bool evenTotal = (_totalDisks % 2 == 0);
    bool evenDisk  = (diskSize % 2 == 0);
    int dir = (evenTotal == evenDisk) ? 1 : -1;  // +1 = 顺时针(0→1→2), -1 = 逆时针

    return (fromIndex + dir + 3) % 3;
}

void AutoDemoSystem::computeNonSmallestMove(entt::registry& reg,
                                             entt::entity& fromNeedle,
                                             entt::entity& toNeedle) const {
    // 找不含最小盘的那两根柱子
    int smallestIdx = findNeedleIndex(reg, _smallest);
    std::pair<int, int> others;
    bool first = true;
    for (int i = 0; i < 3; i++) {
        if (i == smallestIdx) continue;
        if (first) { others.first = i; first = false; }
        else       others.second = i;
    }

    auto needleA = findNeedleByIndex(reg, others.first);
    auto needleB = findNeedleByIndex(reg, others.second);
    auto& stackA = reg.get<NeedleStack>(needleA);
    auto& stackB = reg.get<NeedleStack>(needleB);

    if (stackA.disks.empty()) {
        fromNeedle = needleB; toNeedle = needleA;
    } else if (stackB.disks.empty()) {
        fromNeedle = needleA; toNeedle = needleB;
    } else {
        auto& topA = reg.get<DiskData>(stackA.disks.back());
        auto& topB = reg.get<DiskData>(stackB.disks.back());
        if (topA.size > topB.size) {
            fromNeedle = needleA; toNeedle = needleB;
        } else {
            fromNeedle = needleB; toNeedle = needleA;
        }
    }
}

// ---------- 主体 ----------

void AutoDemoSystem::onUpdate(entt::registry& reg, Resource& res) {
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto& session = reg.get<SessionState>(*sessionView.begin());

    // 非自动演示 → 重置内部状态
    if (!session.isAutoDemo) {
        _moveIndex = 0;
        return;
    }

    // 已完成 → 停
    if (session.completed) {
        _moveIndex = 0;
        return;
    }

    // 动画还在跑 → 等
    if (hasActiveAnimation(reg)) return;

    // 首次 → 初始化
    if (_moveIndex == 0) init(reg);

    // 队列非空 → 等 MoveExecutionSystem 消费
    if (!res.actionQueue.empty()) return;

    // 计算总步数
    int totalMoves = (1 << _totalDisks) - 1;
    if (_moveIndex > totalMoves) {
        _moveIndex = 0;
        return;
    }

    // ── 奇数步：移最小盘 ──
    if (_moveIndex % 2 == 1) {
        int fromIdx = findNeedleIndex(reg, _smallest);
        int toIdx   = nextPeg(fromIdx, 0);  // 最小盘 size=0
        auto from = findNeedleByIndex(reg, fromIdx);
        auto to   = findNeedleByIndex(reg, toIdx);

        MoveAction ma;
        ma.disk = _smallest;
        ma.fromPillar = from;
        ma.toPillar   = to;
        res.actionQueue.push(ma);
    }
    // ── 偶数步：非最小盘的唯一合法移动 ──
    else {
        entt::entity from, to;
        computeNonSmallestMove(reg, from, to);

        auto& fromStack = reg.get<NeedleStack>(from);
        MoveAction ma;
        ma.disk = fromStack.disks.back();
        ma.fromPillar = from;
        ma.toPillar   = to;
        res.actionQueue.push(ma);
    }

    _moveIndex++;
}