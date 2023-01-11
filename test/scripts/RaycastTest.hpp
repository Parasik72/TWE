#pragma once

#include "entity/behavior.hpp"
#include "scene/scene.hpp"
#include "scene/components/components.hpp"

using namespace TWE;

class RaycastTest : public Behavior {
private:
	TransformComponent* tc;
public:
	void start() override {
		
	}

	void update(float deltaTime) override {
		if(input.isMouseButtonPressed(Mouse::MOUSE_BUTTON_LEFT)) {
			tc = getComponent<TransformComponent>();
			glm::vec3 forward = tc->getForward();
			float rayDistace = 100.f;
			RaycastClosestInfo raycastInfo = PhysicsRaycast::raycastClosest(gameObject.getScene(), 
				tc->transform.position, {-forward.x * rayDistace, -forward.y * rayDistace, -forward.z * rayDistace});
			if(raycastInfo.isHit) {
				auto& mrc = raycastInfo.entity.getComponent<MeshRendererComponent>();
				mrc.material.objColor = { 1.f, 0.f, 0.f };
			}
		}
	}
};

extern "C" __declspec(dllexport) Behavior* createRaycastTest();