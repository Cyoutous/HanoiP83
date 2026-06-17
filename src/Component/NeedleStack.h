#pragma once

#include <vector>
#include <entt/entity/fwd.hpp>

struct NeedleStack {
    std::vector<entt::entity> disks;  // 从底到顶
};