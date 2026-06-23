#pragma once

#include "raylib.h"

struct StaticSprite {
    Texture2D texture;
    float width = 0.0f;
    float height = 0.0f;
    Color tint = WHITE;
};
