#include "ResetSystem.h"

#include <chrono>
#include <sstream>
#include <iomanip>

#include "Core/Resource.h"
#include "Core/EntityFactory.h"
#include "Component/SessionState.h"
#include "Component/NextSessionConfig.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"
#include "Component/Position.h"
#include "Component/DiskData.h"
#include "Component/HistoryEntry.h"
#include "Component/Tags.h"
#include "Event/ButtonClickedEvent.h"
#include "Event/SessionStateChangedEvent.h"

std::string_view ResetSystem::name() const {
    return "resetSystem"; 
}

Phase ResetSystem::phase() const { 
    return Phase::FixedUpdate; 
}

// ---------- 事件处理 ----------

void ResetSystem::onButtonClicked(ButtonClickedEvent& event) {
    auto& reg = *_reg;
    auto& res = *_res;

    if (!reg.all_of<ResetButtonTag>(event.button)) return;

    // 找旧 SessionState
    auto sessionView = reg.view<SessionState>();
    if (sessionView.begin() == sessionView.end()) return;
    auto oldSessionEntity = *sessionView.begin();
    auto& oldSession = reg.get<SessionState>(oldSessionEntity);

    // ── 写历史（非自动演示） ──
    if (!oldSession.isAutoDemo) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

        reg.emplace<HistoryEntry>(reg.create(),
            oldSession.diskCount, oldSession.stepCount, oldSession.completed, oss.str()
        );
    }

    // ── 读 NextSessionConfig ──
    auto nextView = reg.view<NextSessionConfig>();
    int newDiskCount = oldSession.diskCount;
    bool newAutoDemo = false;
    if (nextView.begin() != nextView.end()) {
        auto& next = reg.get<NextSessionConfig>(*nextView.begin());
        newDiskCount = next.diskCount;
        newAutoDemo = next.autoDemo;
        next.autoDemo = false;  // 消耗掉自动演示标记
    }

    // 清空所有柱子栈
    auto stackView = reg.view<NeedleStack>();
    for (auto [entity, stack] : stackView.each()) {
        stack.disks.clear();
    }

    // 清空残留的 actionQueue  ???
    while (!res.actionQueue.empty()) res.actionQueue.pop();

    // ── 销毁旧盘子 ──
    auto disks = reg.view<const DiskData>();
    for (auto entity : disks) {
        reg.destroy(entity);
    }

    // ── 销毁旧 SessionState ──
    reg.destroy(oldSessionEntity);

    // ── 创建新 SessionState ──
    auto& factory = res.factory;
    auto newSessionEntity = factory.createSessionState(newDiskCount);
    auto& newSession = reg.get<SessionState>(newSessionEntity);
    newSession.isAutoDemo = newAutoDemo;

    // ── 左柱重建盘子 ──
    auto needleView = reg.view<const NeedleIndex>();
    entt::entity leftNeedle = entt::null;
    for (auto [entity, idx] : needleView.each()) {
        if (idx.index == 0) { leftNeedle = entity; break; }
    }

    if (leftNeedle != entt::null) {
        auto& leftPos = reg.get<Position>(leftNeedle);
        auto& leftStack = reg.get<NeedleStack>(leftNeedle);

        for (int i = 0; i < newDiskCount; i++) {
            int diskIndex = i;
            float diskY = leftPos.y + 129.0f - i * 22.0f;
            auto disk = factory.createDisk(leftPos.x, diskY, diskIndex, newDiskCount);
            leftStack.disks.push_back(disk);
        }
    }

    // ── 通知 ──
    res.events.trigger<SessionStateChangedEvent>();
}

// ---------- 生命周期 ----------

void ResetSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    _res = &res;
    res.events.sink<ButtonClickedEvent>()
        .connect<&ResetSystem::onButtonClicked>(*this);
}

void ResetSystem::onUpdate(entt::registry& reg, Resource& res) {
    // 所有逻辑在事件回调中完成
}