#ifndef RESOURCE_H
#define RESOURCE_H

#include <entt/entt.hpp>

#include "WindowState.h"

struct Resource
{

    entt::dispatcher events;
    WindowState windowstate;
    float dt;
    
};

#endif