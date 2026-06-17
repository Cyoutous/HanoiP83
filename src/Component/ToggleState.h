#pragma once

enum class ToggleVisual { Off, On };

struct ToggleState {
    ToggleVisual visual = ToggleVisual::Off;
};