#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include <glad.h>
#include <memory>
#include <vector>
#include <string>

#include "renderer/vao.hpp"
#include "renderer/vbo.hpp"
#include "renderer/ebo.hpp"
#include "renderer/texture.hpp"

namespace TWE {
    class MeshComponent {
    public:
        MeshComponent() = default;
        MeshComponent(float* vertices, int vertSize, uint32_t* indices, int indSize, const std::string& registryId, const TextureAttachmentSpecification& textureAtttachments = {});
        MeshComponent(float* vertices, int vertSize, uint32_t* indices, int indSize, const std::string& registryId, Texture* texture);
        MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, const TextureAttachmentSpecification& textureAtttachments = {});
        MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, Texture* texture);
        MeshComponent(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId, std::shared_ptr<Texture> texture);
        MeshComponent(const MeshComponent& mesh);
        void setMesh(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, const std::string& registryId);
        void setModelPath(const std::string& modelPath);
        std::shared_ptr<VAO> vao;
        std::shared_ptr<VBO> vbo;
        std::shared_ptr<EBO> ebo;
        std::shared_ptr<Texture> texture;
        std::string modelPath;
        std::string registryId;
    private:
        void create(GLfloat* vertices, GLsizei vertSize, GLuint* indices, GLsizei indSize);
    };
}

#endif