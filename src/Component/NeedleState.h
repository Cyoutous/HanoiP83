#pragma once

enum class NeedleVisual { Idle, Selected, InvalidTarget };

struct NeedleState {
    NeedleVisual visual = NeedleVisual::Idle;
    float autoDeselectTimer = 0.0f;
};