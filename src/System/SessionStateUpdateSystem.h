#pragma once

#include <entt/entt.hpp>

#include "Core/ISystem.h"

#include "Event/SessionStateChangedEvent.h"
#include "Event/BestRecordChangedEvent.h"

struct Resource;

class SessionStateUpdateSystem : public ISystem {

    entt::registry* _reg = nullptr; // AI写的指针

    void onSessionStateChanged(SessionStateChangedEvent&);
    void onBestRecordChanged(BestRecordChangedEvent&);

public:

    std::string_view name() const override;
    Phase phase() const override;

    void onStart(entt::registry& reg, Resource& res) override;
    void onUpdate(entt::registry& reg, Resource& res) override;

};