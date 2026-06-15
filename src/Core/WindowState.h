#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H

#include "raylib.h"

struct WindowState
{
    int logicWidth = 1280;       // 逻辑宽度 (固定)
    int logicHeight = 720;      // 逻辑高度 (固定)

    float scale = 1.0f;          // 缩放比例

    Vector2 offset = { 0.0f, 0.0f };       // 黑边偏移

    RenderTexture2D target; // 渲染目标

    WindowState();
    ~WindowState();
    void update();
};

#endif