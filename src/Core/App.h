#ifndef APP_H
#define APP_H

#include <entt/entt.hpp>
#include "Resource.h"
#include "EntityFactory.h"
#include "SystemScheduler.h"

class App {

    entt::registry reg;
    Resource res;
    
    SystemScheduler scheduler;

public:
    App();
    void run();
};

#endif