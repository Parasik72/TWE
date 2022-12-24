#include "scene/components/mesh-renderer-component.hpp"

namespace TWE {
    MeshRendererComponent::MeshRendererComponent(): showCollider(false) {}

    MeshRendererComponent::MeshRendererComponent(const char* vertexShaderPath, const char* fragmentShaderPath, int entityId, const std::string& registryId)
    : registryId(registryId), entityId(entityId) {
        shader = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);
        colliderShader = std::make_shared<Shader>(SHADER_PATHS[ShaderIndices::COLLIDER_VERT], SHADER_PATHS[ShaderIndices::COLLIDER_FRAG]);
        shader->setUniform("id", entityId);
        colliderShader->setUniform("id", entityId);
        showCollider = false;
    }

    MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& meshRendererComponent) {
        this->material = meshRendererComponent.material;
        this->shader = meshRendererComponent.shader;
        this->registryId = meshRendererComponent.registryId;
        this->entityId = meshRendererComponent.entityId;
        this->colliderShader = meshRendererComponent.colliderShader;
        this->showCollider = meshRendererComponent.showCollider;
    }

    void MeshRendererComponent::setShader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string& registryId) {
        shader = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);
        this->registryId = registryId;
        shader->setUniform("id", entityId);
    }

    void MeshRendererComponent::updateMaterialUniform() {
        shader->setUniform("material.objColor", material.objColor);
        shader->setUniform("material.ambient", material.ambient);
        shader->setUniform("material.diffuse", material.diffuse);
        shader->setUniform("material.specular", material.specular);
        shader->setUniform("material.shininess", material.shininess);
    }
}