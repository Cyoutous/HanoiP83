#pragma once

#include <entt/entt.hpp>

struct Resource;

namespace SaveManager {

    void load(Resource& res);
    void save(entt::registry& reg);

}