#pragma once

#include "entity/behavior.hpp"
#include "scene/scene.hpp"
#include "scene/components/components.hpp"

using namespace TWE;

class CollisionDetectionTest : public Behavior {
private:
	MeshRendererComponent* mrc;
public:
	void start() override {
		
	}

	void update(float deltaTime) override {
		
	}

	void collisionDetection(Entity collidedEntity, const btCollisionObject* collisionObj) override {
		mrc = getComponent<MeshRendererComponent>();
		mrc->material.objColor = {1.f, 0.f, 0.f};
	}
};

extern "C" __declspec(dllexport) Behavior* createCollisionDetectionTest();