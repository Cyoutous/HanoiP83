#pragma once

#include <entt/entt.hpp>

struct MoveAction {
    entt::entity disk;
    entt::entity fromPillar;
    entt::entity toPillar;
};
