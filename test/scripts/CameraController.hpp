#pragma once

#include "entity/behavior.hpp"
#include "scene/scene.hpp"
#include "scene/components/components.hpp"

using namespace TWE;

class CameraController : public Behavior {
private:
    TransformComponent* transform = nullptr;
    float speed = 5.f;
public:
    void start() override {

    }

    void update(float deltaTime) override {
        transform = getComponent<TransformComponent>();
        if(input.isMouseButtonPressed(Mouse::MOUSE_BUTTON_LEFT)) {
			glm::vec3 forward = transform->getForward();
			float rayDistace = 100.f;
			RaycastClosestInfo raycastInfo = PhysicsRaycast::raycastClosest(gameObject.getScene(), 
				transform->transform.position, {-forward.x * rayDistace, -forward.y * rayDistace, -forward.z * rayDistace});
			if(raycastInfo.isHit) {
                if(raycastInfo.entity.hasComponent<MeshRendererComponent>()) {
                    auto& mrc = raycastInfo.entity.getComponent<MeshRendererComponent>();
                    mrc.material.objColor = { 1.f, 0.f, 0.f };
                }
			}
		}
        move(deltaTime);
    }

    void move(float deltaTime) {
        if(input.isKeyPressed(Keyboard::KEY_W))
            transform->move({0.f, 0.f, -speed * deltaTime});
        if(input.isKeyPressed(Keyboard::KEY_S))
            transform->move({0.f, 0.f, speed * deltaTime});
        if(input.isKeyPressed(Keyboard::KEY_D))
            transform->move({speed * deltaTime, 0.f, 0.f});
        if(input.isKeyPressed(Keyboard::KEY_A))
            transform->move({-speed * deltaTime, 0.f, 0.f});
    }
};

extern "C" __declspec(dllexport) Behavior* createCameraController();