#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <string>
#include <vector>

namespace entt {
    class registry;
}
class Resource;

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual std::string_view name() const = 0;
    virtual std::vector<std::string_view> scenes() const { return {"gameplay"}; }

    virtual void on_start(entt::registry& reg, Resource& res) {};
    virtual void on_update(entt::registry& reg, Resource& res) = 0;
    virtual void on_stop(entt::registry& reg, Resource& res) {};
};

#endif