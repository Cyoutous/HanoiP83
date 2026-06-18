#pragma once

#include <entt/entt.hpp>

struct Resource;

class SceneSetup {
public:

    static void build(entt::registry& reg, Resource& res);

};