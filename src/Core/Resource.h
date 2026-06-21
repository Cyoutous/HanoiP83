#ifndef RESOURCE_H
#define RESOURCE_H

#include <entt/entt.hpp>
#include <queue>

#include "WindowState.h"
#include "EntityFactory.h"
#include "Type/moveAction.h"

struct Resource
{

    entt::dispatcher events;
    WindowState windowState;
    EntityFactory  factory;

    std::queue<MoveAction> actionQueue;

    float dt          = 0;             // 渲染帧间隔
    float fixedDt     = 1.0f / 60.0f;  // 逻辑步长
    float accumulator = 0;             // 累计未消费时间
    float alpha       = 0;             // 插值因子，渲染 System 用

    float diskHeight      = 30.0f;     // 盘子厚度
    float minDiskWidth = 60.0f;
    float maxDiskWidth = 240.0f;
    float diskFloatOffset = 30.0f;     
    float needleHeight    = 280.0f;
    float diskBaseOffset  = needleHeight / 2.0f - diskHeight/ 2.0f;
    
    Resource(entt::registry& reg)
        : factory(reg, *this) {};
};

#endif