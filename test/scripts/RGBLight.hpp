#pragma once

#include "entity/behavior.hpp"
#include "scene/scene.hpp"
#include "scene/components/components.hpp"

using namespace TWE;

class RGBLight : public Behavior {
private:
    LightComponent* light = nullptr;
    float time = 0.f;
public:
    void start() override {
        
    }

    void update(float deltaTime) override {
        light = getComponent<LightComponent>();
        time += deltaTime;
        light->color = {cos(time) / 2 + 0.5f, sin(time) / 2 + 0.5f, -cos(time) / 2 + 0.5f};
    }
};

extern "C" __declspec(dllexport) Behavior* createRGBLight();