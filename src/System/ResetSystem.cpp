#include "ResetSystem.h"

#include <chrono>
#include <sstream>
#include <iomanip>

#include "Core/Resource.h"
#include "Core/EntityFactory.h"
#include "Component/SessionState.h"
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

    // ── 写历史 ──
    if (!oldSession.isAutoDemo) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

        reg.emplace<HistoryEntry>(reg.create(),
            oldSession.diskCount, oldSession.stepCount, oldSession.completed, oss.str()
        );
    }

    // ── 读 Tag 定新配置 ──
    int newDiskCount = oldSession.diskCount;
    if (reg.all_of<DiskCountChangedTag>(oldSessionEntity)) {
        newDiskCount = oldSession.diskCount; // TODO: 这个值需来自实际修改后的数字
    }

    bool newAutoDemo = reg.all_of<NextAutoDemoTag>(oldSessionEntity);

    // ── 销毁旧盘子 ──
    auto disks = reg.view<const DiskData>();
    std::vector<entt::entity> toDestroy(disks.begin(), disks.end());
    for (auto entity : toDestroy) {
        reg.destroy(entity);
    }

    // ── 销毁旧 SessionState ──
    reg.destroy(oldSessionEntity);

    // ── 创建新 SessionState ──
    auto& factory = res.factory;
    auto newSessionEntity = factory.createSessionState(newDiskCount);
    auto& newSession = reg.get<SessionState>(newSessionEntity);
    newSession.isAutoDemo = newAutoDemo;

    // ── 找左柱，重建盘子 ──
    auto needleView = reg.view<const NeedleIndex>();
    entt::entity leftNeedle = entt::null;
    for (auto [entity, idx] : needleView.each()) {
        if (idx.index == 0) { leftNeedle = entity; break; }
    }

    if (leftNeedle != entt::null) {
        auto& leftPos = reg.get<Position>(leftNeedle);
        auto& leftStack = reg.get<NeedleStack>(leftNeedle);
        leftStack.disks.clear();

        for (int i = 0; i < newDiskCount; i++) {
            int diskIndex = i;
            float baseY = leftPos.y + 129.0f;
            float diskY = baseY - i * 22.0f;

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