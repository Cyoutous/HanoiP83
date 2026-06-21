#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

struct Resource;

class NeedleInteractionSystem : public ISystem {

    entt::entity _selectedNeedle = entt::null;

    bool canMove(entt::registry& reg, entt::entity from, entt::entity to) const;
    void selectNeedle(entt::registry& reg, entt::entity needle);
    void deselectNeedle(entt::registry& reg);
    void floatTopDisk(entt::registry& reg, entt::entity needle);
    void landTopDisk(entt::registry& reg, entt::entity needle);

    bool hasActiveAnimation(entt::registry& reg) const;

public:

    std::string_view name() const override;
    Phase phase() const override;
    
    void onUpdate(entt::registry& reg, Resource& res) override;

};