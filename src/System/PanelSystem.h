#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"
#include "Component/Panel.h"
#include "Event/ButtonClickedEvent.h"

struct Resource;

class PanelSystem : public ISystem {
    
    entt::registry* _reg = nullptr;
    void onButtonClicked(ButtonClickedEvent& event);
    
    void openPanel(entt::entity panel);
    void closePanel(entt::entity panel);
    entt::entity findOpenPanel();
    entt::entity findPanelByType(PanelType type);

public:

    std::string_view name() const override;
    Phase phase() const override;

    void onStart(entt::registry& reg, Resource& res) override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};