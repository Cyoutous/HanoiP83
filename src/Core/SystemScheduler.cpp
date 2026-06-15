#include "SystemScheduler.h"

#include "ISystem.h"
#include "Resource.h"

SystemScheduler::SystemScheduler(entt::registry& reg, Resource& res)
    : reg(reg), res(res) {
}

void SystemScheduler::add(std::unique_ptr<ISystem> sys) {
    // 根据 sys->scenes() 自动归类建表
    for (auto& scene : sys->scenes()) {
        scene_systems[std::string(scene)].push_back( std::string(sys->name()) );
    }
    index[ std::string(sys->name()) ] = sys.get();
    systems.push_back(std::move(sys));
}

void SystemScheduler::enterScene(std::string_view name) {
    // 1. 停掉当前场景的所有 System
    for (auto& [_, sys] : index) {
        if (sys->enabled() == true) {
            sys->onStop(reg, res);
            sys->setEnabled(false);
        }
    }

    // 2. 查表，启用新场景需要的
    for (auto& sys_name : scene_systems[std::string(name)]) {
        index[sys_name]->setEnabled(true);
        index[sys_name]->onStart(reg, res);  // 首次激活钩子
    }

    current_scene = name;
}

void SystemScheduler::runLogic() {
    for (auto& sys : systems) {
        if (sys->enabled() && sys->phase() == Phase::Logic)
            sys->onUpdate(reg, res);
    }
}

void SystemScheduler::runFixedUpdate() {
    for (auto& sys : systems) {
        if (sys->enabled() && sys->phase() == Phase::FixedUpdate)
            sys->onUpdate(reg, res);
    }
}

void SystemScheduler::runRender() {
    for (auto& sys : systems) {
        if (sys->enabled() && sys->phase() == Phase::Render)
            sys->onUpdate(reg, res);
    }
}