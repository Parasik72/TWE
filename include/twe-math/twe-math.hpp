#ifndef TWE_MATH_HPP
#define TWE_MATH_HPP

#define EPSILON 1.192092896e-07F
#define PI 3.14159265359f

#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "math.h"

namespace TWE {
	class Math {
	public:
		static bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
			glm::mat4 LocalMatrix(transform);
			if(glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
				return false;
			if(glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
			   glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
			   glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), glm::epsilon<float>())) 
			{
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
				LocalMatrix[3][3] = static_cast<float>(1);
			}
			translation = glm::vec3(LocalMatrix[3]);
			LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);
			glm::vec3 Row[3];
			for(glm::length_t i = 0; i < 3; ++i)
			for(glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];
			scale.x = glm::length(Row[0]);
			Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));
			scale.y = glm::length(Row[1]);
			Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));
			scale.z = glm::length(Row[2]);
			Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));
			rotation.y = asin(-Row[0][2]);
			if(cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			} else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}
			return true;
		} 
		[[nodiscard]] static float getAcos(float x) {
        	if (x < -1.f)
				x = -1.f;
			if (x > 1.f)
				x = 1.f;
			return acosf(x);
    	}
		[[nodiscard]] static float getAngle(const glm::quat &quat) {
			return 2.f * getAcos(quat.w);
		}
		[[nodiscard]] static glm::vec3 getAxis(const glm::quat &quat) {
			float squared = 1.f - quat.w * quat.w;
			if (squared < 10.f * EPSILON)
				return {1.f, 0.f, 0.f};           
			float s = 1.f / sqrt(squared);
			return { quat.x * s, quat.y * s, quat.z * s };
		}
	};
}

#endif