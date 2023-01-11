#pragma once

#include "entity/behavior.hpp"
#include "scene/scene.hpp"
#include "scene/components/components.hpp"

using namespace TWE;

class RotateRGBCube : public Behavior {
private:
    TransformComponent* transform = nullptr;
    MeshRendererComponent* meshRendererComponent = nullptr;
    float time = 0.f;
public:
    void start() override {
        
    }

    void update(float deltaTime) override {
        transform = getComponent<TransformComponent>();
        meshRendererComponent = getComponent<MeshRendererComponent>();
        time += deltaTime;
        transform->rotate(100.f * deltaTime, {0.f, 0.f, 1.f});
        meshRendererComponent->material.objColor = {cos(time) / 2 + 0.5f, sin(time) / 2 + 0.5f, -cos(time) / 2 + 0.5f};
    }
};

extern "C" __declspec(dllexport) Behavior* createRotateRGBCube();