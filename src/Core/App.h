#ifndef APP_H
#define APP_H

#include <entt/entt.hpp>
#include "Resource.h"

class App {

    entt::registry reg;
    Resource res;
    //...

public:
    App();
    void run();
};

#endif