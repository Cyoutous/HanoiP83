#pragma once

#include <unordered_map>

struct BestRecord {
    std::unordered_map<int, int> record;  // diskCount → bestSteps
    std::unordered_map<int, float> bestTimes;  // diskCount → seconds

};