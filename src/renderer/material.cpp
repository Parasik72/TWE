#include "renderer/material.hpp"

namespace TWE {
    Material::Material(const glm::vec3& objColor, float ambient, float diffuse, float specular, float shininess)
    : objColor(objColor), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess){}

    Material::Material(const Material& material) {
        this->objColor = material.objColor;
        this->ambient = material.ambient;
        this->diffuse = material.diffuse;
        this->specular = material.specular;
        this->shininess = material.shininess;
    }

    Material& Material::operator=(const Material& material) {
        this->objColor = material.objColor;
        this->ambient = material.ambient;
        this->diffuse = material.diffuse;
        this->specular = material.specular;
        this->shininess = material.shininess;
        return *this;
    }

    bool Material::operator==(const Material& material) {
        return this->objColor == material.objColor
            && this->ambient == material.ambient
            && this->diffuse == material.diffuse
            && this->specular == material.specular
            && this->shininess == material.shininess;
    }

    bool Material::operator!=(const Material& material) {
        return !(*this == material);
    }
}