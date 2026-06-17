#pragma once

#include <vector>
#include <entt/entt.hpp>

struct NeedleStack {
    std::vector<entt::entity> disks;  // 从底到顶
};