#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <glfw3.h>

namespace TWE {
    enum Mouse {
        MOUSE_BUTTON_1      = GLFW_MOUSE_BUTTON_1      ,
        MOUSE_BUTTON_2      = GLFW_MOUSE_BUTTON_2      ,
        MOUSE_BUTTON_3      = GLFW_MOUSE_BUTTON_3      ,
        MOUSE_BUTTON_4      = GLFW_MOUSE_BUTTON_4      ,
        MOUSE_BUTTON_5      = GLFW_MOUSE_BUTTON_5      ,
        MOUSE_BUTTON_6      = GLFW_MOUSE_BUTTON_6      ,
        MOUSE_BUTTON_7      = GLFW_MOUSE_BUTTON_7      ,
        MOUSE_BUTTON_8      = GLFW_MOUSE_BUTTON_8      ,
        MOUSE_BUTTON_LAST   = GLFW_MOUSE_BUTTON_LAST   ,
        MOUSE_BUTTON_LEFT   = GLFW_MOUSE_BUTTON_LEFT   ,
        MOUSE_BUTTON_RIGHT  = GLFW_MOUSE_BUTTON_RIGHT  ,
        MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE ,
    };
}

#endif