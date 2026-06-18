#include "TweenSystem.h"

#include <algorithm>
#include <cmath>

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/TweenPosition.h"
#include "Component/TweenSequence.h"

std::string_view TweenSystem::name() const {
    return "TweenSystem"; 
}

Phase TweenSystem::phase() const {
    return Phase::Logic;
}

void TweenSystem::onUpdate(entt::registry& reg, Resource& res) {
    float dt = res.dt;

    // === 第 1 遍：TweenSequence → 创建 TweenPosition ===
    auto seqView = reg.view<TweenSequence>();
    for (auto [entity, seq] : seqView.each()) {

        // 当前段还在跑 — 不动
        if (reg.all_of<TweenPosition>(entity)) continue;

        // 等 delay
        if (seq.currentDelay > 0.0f) {
            seq.currentDelay -= dt;
            continue;
        }

        // 队列空 — 序列结束
        if (seq.steps.empty()) {
            reg.remove<TweenSequence>(entity);
            continue;
        }

        // peek 下一步的 startDelay（不 pop）
        float stepDelay = seq.steps.front().startDelay;
        if (stepDelay > 0.0f) {
            seq.currentDelay = stepDelay;
            seq.steps.front().startDelay = 0.0f;  // 标记延迟已消费
            continue;
        }

        // 真正消费这一步
        auto config = seq.steps.front();
        seq.steps.pop_front();

        auto& pos = reg.get<Position>(entity);
        reg.emplace<TweenPosition>(entity,
            pos.x, pos.y,
            config.targetX, config.targetY,
            config.duration, 0.0f,
            config.mode
        );


    }

    // === 第 2 遍：所有 TweenPosition → 更新 Position ===
    auto tweenView = reg.view<Position, TweenPosition>();
    for (auto [entity, pos, tween] : tweenView.each()) {
        tween.elapsed += dt;
        float t = std::min(tween.elapsed / tween.duration, 1.0f);
        float e = ease(t, tween.mode);
        pos.x = tween.startX + (tween.targetX - tween.startX) * e;
        pos.y = tween.startY + (tween.targetY - tween.startY) * e;

        if (t >= 1.0f) {
            pos.x = tween.targetX;
            pos.y = tween.targetY;
            reg.remove<TweenPosition>(entity);
        }
    }
}

float TweenSystem::ease(float t, EaseMode mode) {
    switch (mode) {
    case EaseMode::Instant:   return 1.0f;
    case EaseMode::Linear:    return t;
    case EaseMode::EaseIn:    return t * t;
    case EaseMode::EaseOut:   return 1.0f - (1.0f - t) * (1.0f - t);
    case EaseMode::EaseInOut: return t < 0.5f ? 2.0f * t * t
                                : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
    }
    return t;
}