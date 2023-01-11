#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glad.h>
#include <glm.hpp>

namespace TWE {
    class Material {
    public:
        Material(const glm::vec3& objColor = {1.f, 1.f, 1.f},
                float ambient = 0.01f, 
                float diffuse = 0.5f, 
                float specular = 1.f,
                float shininess = 64.f);
        Material(const Material& material);
        Material& operator=(const Material& material);
        bool operator==(const Material& material);
        bool operator!=(const Material& material);
        glm::vec3 objColor;
        float ambient;
        float diffuse;
        float specular;
        float shininess;
    };
}

#endif