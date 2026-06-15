#include <vector>
#include <string>

#include "raylib.h"

#include "Core/App.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); //允许缩放
    InitWindow(1280, 720, "Game");   // raylib就绪
    SetTargetFPS(0);                  // 不锁帧，让插值生效

    {
        App app; //创建 App（内部构造 WindowState → LoadRenderTexture）
        app.run(); //主循环
    }

    CloseWindow(); //app 析构 → WindowState 析构 → UnloadRenderTexture

    return 0;
}
