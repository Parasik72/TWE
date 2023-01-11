#include "scene/components/mesh-component.hpp"

namespace TWE {
    MeshComponent::MeshComponent(GLfloat* vertices, GLsizei vertSize, GLuint* indices, GLsizei indSize, const std::string& registryId, const TextureAttachmentSpecification& textureAtttachments)
    : registryId(registryId) {
        create(vertices, vertSize, indices, indSize);
        texture = std::make_shared<Texture>(textureAtttachments);
    }

    MeshComponent::MeshComponent(GLfloat* vertices, GLsizei vertSize, GLuint* indices, GLsizei indSize, const std::string& registryId, Texture* texture)
    : registryId(registryId) {
        create(vertices, vertSize, indices, indSize);
        this->texture = std::make_shared<Texture>(*texture);
    }

    MeshComponent::MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, const TextureAttachmentSpecification& textureAtttachments)
    : vao(vao), vbo(vbo), ebo(ebo), registryId(registryId) {
        texture = std::make_shared<Texture>(textureAtttachments);
    }

    MeshComponent::MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, Texture* texture)
    : vao(vao), vbo(vbo), ebo(ebo), registryId(registryId) {
        this->texture = std::make_shared<Texture>(*texture);
    }

    MeshComponent::MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, std::shared_ptr<Texture> texture) 
     : vao(vao), vbo(vbo), ebo(ebo), registryId(registryId), texture(texture) {}

    MeshComponent::MeshComponent(const MeshComponent& mesh) {
        this->vao = mesh.vao;
        this->vbo = mesh.vbo;
        this->ebo = mesh.ebo;
        this->texture = mesh.texture;
        this->registryId = mesh.registryId;
        this->modelPath = mesh.modelPath;
    }

    void MeshComponent::setMesh(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId) {
        this->vao = vao;
        this->vbo = vbo;
        this->ebo = ebo;
        this->registryId = registryId;
    }

    void MeshComponent::setModelPath(const std::string& modelPath) {
        this->modelPath = modelPath;
    }

    void MeshComponent::create(GLfloat* vertices, GLsizei vertSize, GLuint* indices, GLsizei indSize) {
        vao = std::make_shared<VAO>();
        vao->bind();
        vbo = std::make_shared<VBO>(vertices, vertSize, GL_STATIC_DRAW);
        vbo->bind();
        ebo = std::make_shared<EBO>(indices, indSize, GL_STATIC_DRAW);
        ebo->bind();
        vao->setAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0, *vbo.get());
        vao->setAttrib(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)), *vbo.get());
        vao->setAttrib(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)), *vbo.get());
        vao->unbind();
        vbo->unbind();
        ebo->unbind();
    }
}