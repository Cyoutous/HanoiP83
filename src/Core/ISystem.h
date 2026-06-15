#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <string>
#include <vector>

namespace entt {
    class registry;
}
class Resource;

enum Phase {
    Logic, Render
}; //逻辑帧执行或者渲染帧执行

class ISystem {

    bool enabled_status = false;

public:

    virtual ~ISystem() = default;

    virtual std::string_view name() const = 0; //系统名称

    virtual std::vector<std::string_view> scenes() const { return {"gameplay"}; } //所属游戏场景
    virtual Phase phase() const { return Phase::Logic; } //执行时机

    virtual void on_start(entt::registry& reg, Resource& res) {};
    virtual void on_update(entt::registry& reg, Resource& res) = 0;
    virtual void on_stop(entt::registry& reg, Resource& res) {};

    bool enabled() const { return enabled_status; }
    void set_enabled(bool value) { enabled_status = value; }
};

#endif