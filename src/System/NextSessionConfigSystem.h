#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

struct Resource;

class NextSessionConfigSystem : public ISystem {

    entt::registry* _reg = nullptr;

    void onButtonClicked(struct ButtonClickedEvent& event);
    void onToggleChanged(struct ToggleChangedEvent& event);
    void setDiskCount(entt::registry& reg, int count);

public:

    std::string_view name() const override;
    Phase phase() const override;

    void onStart(entt::registry& reg, Resource& res) override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};