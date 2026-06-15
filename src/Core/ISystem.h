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

    bool enabledStatus = false;

public:

    virtual ~ISystem() = default;

    virtual std::string_view name() const = 0; //系统名称

    virtual std::vector<std::string_view> scenes() const { return {"gameplay"}; } //所属游戏场景
    virtual Phase phase() const { return Phase::Logic; } //执行时机

    virtual void onStart(entt::registry& reg, Resource& res) {};
    virtual void onUpdate(entt::registry& reg, Resource& res) = 0;
    virtual void onStop(entt::registry& reg, Resource& res) {};

    bool enabled() const { return enabledStatus; }
    void setEnabled(bool value) { enabledStatus = value; }
};

#endif