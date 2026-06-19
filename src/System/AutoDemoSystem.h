#pragma once
#include <entt/entt.hpp>
#include "Core/ISystem.h"

struct Resource;

class AutoDemoSystem : public ISystem {
    
    int _moveIndex = 0;
    int _totalDisks = 0;
    entt::entity _smallest = entt::null;
    
    void init(entt::registry& reg);
    int  findNeedleIndex(entt::registry& reg, entt::entity disk) const;
    entt::entity findNeedleByIndex(entt::registry& reg, int index) const;
    bool hasActiveAnimation(entt::registry& reg) const;
    int  nextPeg(int fromIndex, int diskSize) const;
    void computeNonSmallestMove(entt::registry& reg, entt::entity& from, entt::entity& to) const;

public:

    std::string_view name() const override;
    Phase phase() const override;
    
    void onUpdate(entt::registry& reg, Resource& res) override;

};