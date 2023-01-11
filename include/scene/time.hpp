#ifndef TIME_HPP
#define TIME_HPP

#include <glfw3.h>

namespace TWE {
    class Time{
    public:
        Time();
        static float getTime();
        static float getDeltaTime();
    private:
        static void calculate();
        static float* fps;
        static float* deltaTime;
        static float* curTime;
        static float* preTime;
        friend class Engine;
        friend class Behavior;
        friend class ScriptComponent;
    };
}

#endif