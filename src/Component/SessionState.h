#pragma once

struct SessionState {
    int diskCount = 3;
    int stepCount = 0;
    bool isAutoDemo = false;
    bool completed = false;

    float elapsedTime = 0.0f;
    bool timerRunning = false;
    bool timeUp = false;

};