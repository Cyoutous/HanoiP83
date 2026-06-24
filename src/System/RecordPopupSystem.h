#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

struct Resource;

class RecordPopupSystem : public ISystem {
    
    entt::registry* _reg = nullptr;

    void onRecordBroken(struct RecordBrokenEvent& event);
    void onResetClicked(struct ButtonClickedEvent& event);

public:

    std::string_view name() const override;
    Phase phase() const override;
    
    void onStart(entt::registry& reg, Resource& res) override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};
