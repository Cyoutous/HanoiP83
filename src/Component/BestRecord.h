#pragma once

#include <unordered_map>

struct BestRecords {
    std::unordered_map<int, int> record;  // diskCount → bestSteps
};