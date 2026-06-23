#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"
#include "Event/ButtonClickedEvent.h"

struct Resource;

class SettingsSystem : public ISystem {

    entt::registry* _reg = nullptr;
    Resource*       _res = nullptr;

    int _debugClickCount = 0;
    bool _onStartCalled = false;

    void onButtonClicked(ButtonClickedEvent& event);
    //void onSettingsButtonClicked(SettingsButtonEvent& event);
    void populatePending(entt::registry& reg);
    void closePanel(entt::entity panel);
    entt::entity findPanel() const;
    void updateDisplay(entt::registry& reg);

public:

    std::string_view name() const override;
    Phase phase() const override;
    void onStart(entt::registry& reg, Resource& res) override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};