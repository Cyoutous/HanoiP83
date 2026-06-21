#pragma once

#include "raylib.h"

struct Clickable {
    Vector2 clickOffset = {0, 0};
    float width = 0;
    float height = 0;
};