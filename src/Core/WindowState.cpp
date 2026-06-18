#include "WindowState.h"

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

Vector2 WindowState::toLogic(float screenX, float screenY) const {
    return {
        (screenX - offset.x) / scale,
        (screenY - offset.y) / scale
    };
}