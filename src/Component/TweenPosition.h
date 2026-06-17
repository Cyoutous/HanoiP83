#pragma once

enum class EaseMode { Instant, Linear, EaseIn, EaseOut, EaseInOut };

struct TweenPosition {
    float startX = 0.0f;
    float startY = 0.0f;
    float targetX = 0.0f;
    float targetY = 0.0f;
    float duration = 0.0f;
    float elapsed = 0.0f;
    EaseMode mode = EaseMode::Linear;
};