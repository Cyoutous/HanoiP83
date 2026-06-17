#pragma once

#include <string>
#include "raylib.h"

struct TextLabel {
    std::string text;
    int fontSize = 20;
    Color color = WHITE;
};