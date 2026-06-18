#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"
#include "Component/TweenPosition.h"

struct Resource;

class TweenSystem : public ISystem {

    static float ease(float t, EaseMode mode);

public:

    std::string_view name() const override;
    Phase phase() const override;

    void onUpdate(entt::registry& reg, Resource& res) override;
};