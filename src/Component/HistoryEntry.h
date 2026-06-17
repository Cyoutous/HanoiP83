#pragma once
#include <string>

struct HistoryEntry {
    int diskCount = 0;
    int steps = 0;
    bool completed = false;
    std::string timestamp;
};