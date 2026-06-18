#pragma once
#include <entt/entt.hpp>
#include "Core/ISystem.h"

struct Resource;

class InputSystem : public ISystem {

    entt::entity _pressedEntity = entt::null;

public:

    std::string_view name() const override;
    Phase phase() const override;
    
    void onUpdate(entt::registry& reg, Resource& res) override;
    
};