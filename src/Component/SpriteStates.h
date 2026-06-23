#pragma once

#include <unordered_map>

#include "raylib.h"

struct SpriteStates {
    std::unordered_map<int, Texture2D> textures;
    float width = 0.0f;
    float height = 0.0f;
};