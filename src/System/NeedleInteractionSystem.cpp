#include "NeedleInteractionSystem.h"

#include <algorithm>

#include "Core/Resource.h"
#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/NeedleState.h"
#include "Component/NeedleStack.h"
#include "Component/DiskData.h"
#include "Component/InputResponse.h"
#include "Component/TweenPosition.h"
#include "Component/SessionState.h"
#include "Component/TweenSequence.h"

std::string_view NeedleInteractionSystem::name() const { return "needleInteraction"; }
Phase NeedleInteractionSystem::phase() const { return Phase::Logic; }

// ---------- 辅助 ----------

bool NeedleInteractionSystem::canMove(entt::registry& reg, Resource& res, entt::entity from, entt::entity to) const {
    if (from == to) return false;

    auto& fromStack = reg.get<NeedleStack>(from);
    if (fromStack.disks.empty()) return false;

    auto& toStack = reg.get<NeedleStack>(to);
    if (toStack.disks.empty()) return true;

    auto& movedDisk = reg.get<DiskData>(fromStack.disks.back());
    auto& topDisk   = reg.get<DiskData>(toStack.disks.back());
    return movedDisk.size > topDisk.size;
}

void NeedleInteractionSystem::floatTopDisk(entt::registry& reg, Resource& res, entt::entity needle) {
    auto& stack = reg.get<NeedleStack>(needle);
    if (stack.disks.empty()) return;
    auto disk = stack.disks.back();
    reg.remove<TweenPosition, TweenSequence>(disk);
    auto& pos = reg.get<Position>(disk);
    reg.emplace<TweenPosition>(disk, pos.x, pos.y, pos.x, pos.y - res.diskFloatOffset, 0.08f, 0.0f, EaseMode::EaseOut);
}

void NeedleInteractionSystem::landTopDisk(entt::registry& reg, Resource& res, entt::entity needle) {
    auto& stack = reg.get<NeedleStack>(needle);
    if (stack.disks.empty()) return;
    auto disk = stack.disks.back();
    reg.remove<TweenPosition, TweenSequence>(disk);
    auto& pos = reg.get<Position>(disk);
    reg.emplace<TweenPosition>(disk, pos.x, pos.y, pos.x, pos.y + res.diskFloatOffset, 0.08f, 0.0f, EaseMode::EaseOut);
}

void NeedleInteractionSystem::selectNeedle(entt::registry& reg, Resource& res, entt::entity needle) {
    _selectedNeedle = needle;
    reg.get<NeedleState>(needle).visual = NeedleVisual::Selected;
    floatTopDisk(reg, res, needle);
}

void NeedleInteractionSystem::deselectNeedle(entt::registry& reg, Resource& res) {
    if (_selectedNeedle == entt::null) return;
    auto needle = _selectedNeedle;
    auto& state = reg.get<NeedleState>(needle);
    state.visual = NeedleVisual::Idle;
    state.autoDeselectTimer = 0.0f;
    landTopDisk(reg, res, needle);
    _selectedNeedle = entt::null;
}

bool NeedleInteractionSystem::hasActiveAnimation(entt::registry& reg) const {
    auto view = reg.view<const DiskData>();
    for (auto entity : view) {
        if (reg.all_of<TweenPosition>(entity) || reg.all_of<TweenSequence>(entity))
            return true;
    }
    return false;
}


// ---------- 主体 ----------

void NeedleInteractionSystem::onUpdate(entt::registry& reg, Resource& res) {
    float dt = res.dt;

    // 读 SessionState — 自动演示或已完成时禁交互
    auto sessionView = reg.view<const SessionState>();
    if (sessionView.begin() != sessionView.end()) {
        auto& session = reg.get<const SessionState>(*sessionView.begin());
        if (session.isAutoDemo || session.completed) {
            if (_selectedNeedle != entt::null) deselectNeedle(reg, res);
            return;
        }
    }

    if (hasActiveAnimation(reg)) return;

    // 自动恢复定时器
    auto stateView = reg.view<NeedleState>();
    for (auto [entity, state] : stateView.each()) {
        if (state.autoDeselectTimer > 0.0f) {
            state.autoDeselectTimer -= dt;
            if (state.autoDeselectTimer <= 0.0f) {
                state.visual = NeedleVisual::Idle;
                if (entity == _selectedNeedle) {
                    landTopDisk(reg, res, entity);
                    _selectedNeedle = entt::null;
                }
            }
        }
    }

    // 读 InputResponse — 遍历被点击的柱子
    auto view = reg.view<NeedleState, const NeedleStack, const InputResponse>();
    for (auto [entity, state, stack, input] : view.each()) {
        if (!input.clicked) continue;

        // ── 无选中 ──
        if (_selectedNeedle == entt::null) {
            if (stack.disks.empty()) {
                // 点空柱 → 闪一下
                state.visual = NeedleVisual::Selected;
                state.autoDeselectTimer = 0.15f;
            } else {
                selectNeedle(reg, res, entity);
            }
            continue;
        }

        // ── 已有选中 ──
        if (entity == _selectedNeedle) {
            // 再次点同一根 → 取消
            deselectNeedle(reg, res);
            continue;
        }

        // 点另一根
        if (canMove(reg, res, _selectedNeedle, entity)) {
            // 合法 → 目标柱短暂高光
            auto& targetState = reg.get<NeedleState>(entity);
            targetState.visual = NeedleVisual::Selected;
            targetState.autoDeselectTimer = 0.15f;

            //landTopDisk(reg, _selectedNeedle);  // 落回

            // 塞 MoveAction
            auto disk = reg.get<NeedleStack>(_selectedNeedle).disks.back();
            reg.remove<TweenPosition, TweenSequence>(disk);

            MoveAction action;
            action.disk = disk;
            action.fromPillar = _selectedNeedle;
            action.toPillar = entity;
            res.actionQueue.push(action);

            // 清选中
            auto& fromState = reg.get<NeedleState>(_selectedNeedle);
            fromState.visual = NeedleVisual::Idle;
            _selectedNeedle = entt::null;
        } else {
            // 非法 → 目标柱红闪
            auto& targetState = reg.get<NeedleState>(entity);
            targetState.visual = NeedleVisual::InvalidTarget;
            targetState.autoDeselectTimer = 0.15f;
        }
    }

    // 清理 InputResponse（本帧已消费）
    //reg.clear<InputResponse>();
}