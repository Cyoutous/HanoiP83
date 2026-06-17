#pragma once

enum class ButtonVisual { Idle, Pressed };

struct ButtonState {
    ButtonVisual visual = ButtonVisual::Idle;
};