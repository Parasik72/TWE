#include "scene/time.hpp"

namespace TWE {
	float* Time::fps = new float{0};
	float* Time::deltaTime = new float{0};
	float* Time::curTime = new float{0};
	float* Time::preTime = new float{0};

	void Time::calculate() {
		curTime[0] = glfwGetTime();
		deltaTime[0] = curTime[0] - preTime[0];
		fps[0] = 1.0f / deltaTime[0];
		preTime[0] = curTime[0];
	}

	float Time::getTime() { return curTime[0]; }
	float Time::getDeltaTime() { return deltaTime[0]; }
}