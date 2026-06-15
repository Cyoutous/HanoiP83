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

WindowState::WindowState() {
    target = LoadRenderTexture(logicWidth, logicHeight);
    update();
}

WindowState::~WindowState() {
    UnloadRenderTexture(target);
}

void WindowState::update() {
    int actualWidth = GetScreenWidth();
    int actualHeight = GetScreenHeight();

    // 等比缩放：取宽高比中较小的那个
    float scaleX = (float)actualWidth / logicWidth;
    float scaleY = (float)actualHeight / logicHeight;
    scale = scaleX < scaleY ? scaleX : scaleY;

    offset = { 
        (float)(actualWidth - (int)(logicWidth * scale)) / 2, 
        (float)(actualHeight - (int)(logicHeight * scale)) / 2
    };
}

#endif