#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

struct Resource;

class MoveExecutionSystem : public ISystem {
    
    bool hasActiveTween(entt::registry& reg) const;
    float calcDiskY(entt::registry& reg, Resource& res, entt::entity needle, int stackIndex) const;

public:

    std::string_view name() const override;
    Phase phase() const override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};