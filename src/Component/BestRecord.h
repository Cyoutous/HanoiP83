#pragma once

#include <unordered_map>

struct BestRecord {
    std::unordered_map<int, int> record;  // diskCount → bestSteps
};