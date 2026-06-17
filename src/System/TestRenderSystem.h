#pragma once

#include "Core/ISystem.h"

class TestRenderSystem : public ISystem {
    
public:

    std::string_view name() const override;
    Phase phase() const override;
    
    void onUpdate(entt::registry& reg, Resource& res) override;
    
};