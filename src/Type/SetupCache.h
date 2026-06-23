#pragma once

#include <vector>
#include <unordered_map>

struct NeedleDiskData {
    int diskSize;
};

struct NeedleInitData {
    int index;
    std::vector<int> diskSizes;   // 从底到顶
};

struct SetupCache {
    // SessionState
    int diskCount = 3;
    int stepCount = 0;
    bool isAutoDemo = false;
    bool completed = false;

    // NextSessionConfig
    int nextDiskCount = 3;
    bool nextAutoDemo = false;

    // 三根柱子的盘子排列
    std::vector<NeedleInitData> needles;

    // BestRecord
    std::unordered_map<int, int> bestRecords;

    SetupCache();
};

inline SetupCache::SetupCache() {
    needles = {
        {0, {0, 1, 2}},
        {1, {}},
        {2, {}}
    };

    for (int d = 3; d <= 10; ++d) {
        bestRecords[d] = 0;
    }
}