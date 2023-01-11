#ifndef ACTION_HPP
#define ACTION_HPP

#include <glfw3.h>

namespace TWE {
    enum Action {
        RELEASE = GLFW_RELEASE,            
        PRESS   = GLFW_PRESS  ,            
        REPEAT  = GLFW_REPEAT ,            
    };
}

#endif