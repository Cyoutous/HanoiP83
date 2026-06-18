#ifndef RESOURCE_H
#define RESOURCE_H

#include <entt/entt.hpp>

#include "WindowState.h"
#include "EntityFactory.h"

struct Resource
{

    entt::dispatcher events;
    WindowState windowState;
    EntityFactory  factory;

    float dt          = 0;             // 渲染帧间隔
    float fixedDt     = 1.0f / 60.0f;  // 逻辑步长
    float accumulator = 0;             // 累计未消费时间
    float alpha       = 0;             // 插值因子，渲染 System 用
    
    Resource(entt::registry& reg)
        : factory(reg) {};
};

#endif