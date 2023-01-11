#include "scene/components/mesh-renderer-component.hpp"

namespace TWE {
    MeshRendererComponent::MeshRendererComponent() {}

    MeshRendererComponent::MeshRendererComponent(const char* vertexShaderPath, const char* fragmentShaderPath, int entityId, const std::string& registryId)
    : registryId(registryId), entityId(entityId) {
        shader = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);
        shader->setUniform("id", entityId);
    }

    MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& meshRendererComponent) {
        this->material = meshRendererComponent.material;
        this->shader = meshRendererComponent.shader;
        this->registryId = meshRendererComponent.registryId;
        this->entityId = meshRendererComponent.entityId;
    }

    void MeshRendererComponent::setShader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string& registryId) {
        shader = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);
        this->registryId = registryId;
        shader->setUniform("id", entityId);
    }

    void MeshRendererComponent::setMaterial(const Material& material) {
        this->material = material;
    }

    void MeshRendererComponent::updateMaterialUniform() {
        shader->setUniform("material.objColor", material.objColor);
        shader->setUniform("material.ambient", material.ambient);
        shader->setUniform("material.diffuse", material.diffuse);
        shader->setUniform("material.specular", material.specular);
        shader->setUniform("material.shininess", material.shininess);
    }
}