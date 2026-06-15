#ifndef SYSTEMSCHEDULER_H
#define SYSTEMSCHEDULER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ISystem.h"

class SystemScheduler {

    std::vector<std::unique_ptr<ISystem>> systems; //存储系统类
    std::unordered_map<std::string, ISystem*> index; //系统名映射表
    std::unordered_map<std::string, std::vector<std::string>> scene_systems; //场景映射表
    std::string current_scene; //当前场景

    entt::registry& reg;
    Resource& res;

public:

    SystemScheduler(entt::registry& reg, Resource& res);

    void add(std::unique_ptr<ISystem> sys); //添加系统
    void enterScene(std::string_view name); //进入场景
    void runLogic(); //执行逻辑系统
    void runRender(); //执行渲染系统

};

#endif  