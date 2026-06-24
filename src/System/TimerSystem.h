#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

struct Resource;

class TimerSystem : public ISystem {

public:
    
    std::string_view name() const override;
    Phase phase() const override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};