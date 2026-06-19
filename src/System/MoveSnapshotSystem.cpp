#include "MoveSnapshotSystem.h"

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/Interpolated.h"

std::string_view MoveSnapshotSystem::name() const { 
    return "moveSnapshot"; 
}

Phase MoveSnapshotSystem::phase() const { 
    return Phase::FixedUpdate; 
}

void MoveSnapshotSystem::onUpdate(entt::registry& reg, Resource& res) {
    auto view = reg.view<const Position, Interpolated>();
    for (auto [entity, pos, interp] : view.each()) {
        interp.prevX = pos.x;
        interp.prevY = pos.y;
    }
}