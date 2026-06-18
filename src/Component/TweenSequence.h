#pragma once

#include <queue>

#include "TweenPosition.h"    // 引用 EaseMode

struct TweenConfig {
    float targetX = 0.0f;
    float targetY = 0.0f;
    float duration = 0.0f;
    EaseMode mode = EaseMode::Linear;
    float startDelay = 0.0f;
};

struct TweenSequence {
    std::deque<TweenConfig> steps;
    float currentDelay = 0.0f;
};