#include "scene/shape.hpp"

namespace TWE {
    Registry<MeshSpecification>* Shape::meshRegistry = nullptr;
    Registry<MeshRendererSpecification>* Shape::meshRendererRegistry = nullptr;
    int Shape::meshCounter;
    float Shape::cubeVertices[] = {
        //front
        -0.5f, -0.5f, 0.5f,   0.0f,  0.0f, 1.0f,      0.f, 1.f, //left down     0
        0.5f,  -0.5f, 0.5f,   0.0f,  0.0f, 1.0f,      1.f, 1.f, //right down    1
        0.5f,   0.5f, 0.5f,   0.0f,  0.0f, 1.0f,      1.f, 0.f, //right up      2
        -0.5f,  0.5f, 0.5f,   0.0f,  0.0f, 1.0f,      0.f, 0.f, //left up       3
        //back
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,     1.f, 1.f, //right down    4
        0.5f,  -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,     0.f, 1.f, //left down     5
        0.5f,   0.5f, -0.5f,  0.0f,  0.0f, -1.0f,     0.f, 0.f, //left up       6
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,     1.f, 0.f, //right up      7
        //left
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.f, 1.f, //left down     8
        -0.5f, -0.5f, 0.5f,   -1.0f,  0.0f,  0.0f,    1.f, 1.f, //right down    9
        -0.5f,  0.5f, 0.5f,   -1.0f,  0.0f,  0.0f,    1.f, 0.f, //right up      10
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.f, 0.f, //left up       11
        //right
        0.5f,  -0.5f, 0.5f,   1.0f,  0.0f,  0.0f,     0.f, 1.f, //left down     12
        0.5f,  -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,     1.f, 1.f, //right down    13
        0.5f,   0.5f, -0.5f,  1.0f,  0.0f,  0.0f,     1.f, 0.f, //right up      14
        0.5f,   0.5f, 0.5f,   1.0f,  0.0f,  0.0f,     0.f, 0.f, //left up       15
        //top
        -0.5f,  0.5f, 0.5f,   0.0f,  1.0f,  0.0f,     0.f, 1.f, //left down     16
        0.5f,   0.5f, 0.5f,   0.0f,  1.0f,  0.0f,     1.f, 1.f, //right down    17
        0.5f,   0.5f, -0.5f,  0.0f,  1.0f,  0.0f,     1.f, 0.f, //right up      18
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,     0.f, 0.f, //left up       19
        //bottom
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,     0.f, 1.f, //left down     20
        0.5f,  -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,     1.f, 1.f, //right down    21
        0.5f,  -0.5f, 0.5f,   0.0f, -1.0f,  0.0f,     1.f, 0.f, //right up      22
        -0.5f, -0.5f, 0.5f,   0.0f, -1.0f,  0.0f,     0.f, 0.f, //left up       23
    };
    uint32_t Shape::cubeIndices[] = {
        //front
        0, 1, 2,
        0, 2, 3,
        //back
        4, 6, 5,
        4, 7, 6,
        //left
        8, 9, 10,
        8, 10, 11,
        //right
        12, 13, 14,
        12, 14, 15,
        //top
        16, 17, 18,
        16, 18, 19,
        //bottom
        20, 21, 22,
        20, 22, 23
    };
    float Shape::plateVertices[] = {
        -5.f, 0.f,  5.f,    0.0f,  1.0f,  0.0f,     0.f, 1.f,
        5.f,  0.f,  5.f,    0.0f,  1.0f,  0.0f,     1.f, 1.f,
        5.f,  0.f, -5.f,    0.0f,  1.0f,  0.0f,     1.f, 0.f,
        -5.f, 0.f, -5.f,    0.0f,  1.0f,  0.0f,     0.f, 0.f,
    };
    uint32_t Shape::plateIndices[] = {
        0, 1, 2,
        0, 2, 3
    };
    uint32_t Shape::cubemapIndices[] = {
        //front
        0, 2, 1,
        0, 3, 2,
        //back
        4, 5, 6,
        4, 6, 7,
        //left
        8, 10, 9,
        8, 11, 10,
        //right
        12, 14, 13,
        12, 15, 14,
        //top
        16, 18, 17,
        16, 19, 18,
        //bottom
        20, 22, 21,
        20, 23, 22
    };

    void Shape::reset() {
        meshRegistry->clean();
        meshRendererRegistry->clean();
        meshCounter = 0;
        fillMeshRegistry();
        fillMeshRendererRegistry();
    }

    void Shape::initialize(Registry<MeshSpecification>* meshRegistryT, Registry<MeshRendererSpecification>* meshRendererRegistryT) {
        meshRegistry = meshRegistryT;
        meshRendererRegistry = meshRendererRegistryT;
        meshCounter = 0;
        fillMeshRegistry();
        fillMeshRendererRegistry();
    }

    void Shape::fillMeshRegistry() {
        // Cube mesh
        std::string cubeMeshId = "Cube mesh";
        auto cubeMeshComponent = MeshComponent(Shape::cubeVertices, sizeof(Shape::cubeVertices), 
            Shape::cubeIndices, sizeof(Shape::cubeIndices), cubeMeshId, TextureAttachmentSpecification{});
        registerMeshSpecification(cubeMeshComponent.vao, cubeMeshComponent.vbo, cubeMeshComponent.ebo, EntityCreationType::Cube, "", cubeMeshId);
        // Plate mesh
        std::string plateMeshId = "Plate mesh";
        auto plateMeshComponent = MeshComponent(Shape::plateVertices, sizeof(Shape::plateVertices), 
            Shape::plateIndices, sizeof(Shape::plateIndices), plateMeshId, TextureAttachmentSpecification{});
        registerMeshSpecification(plateMeshComponent.vao, plateMeshComponent.vbo, plateMeshComponent.ebo, EntityCreationType::Plate, "", plateMeshId);
        // Cubemap mesh
        std::string cubemapMeshId = "Cubemap mesh";
        auto cubemapMeshComponent = MeshComponent(Shape::cubeVertices, sizeof(Shape::cubeVertices), 
            Shape::cubemapIndices, sizeof(Shape::cubemapIndices), cubemapMeshId, TextureAttachmentSpecification{});
        registerMeshSpecification(cubemapMeshComponent.vao, cubemapMeshComponent.vbo, cubemapMeshComponent.ebo, EntityCreationType::Cubemap, "", cubemapMeshId);
    }

    void Shape::fillMeshRendererRegistry() {
        // Default renderer
        std::string defaultMeshRendererId = "Default renderer";
        registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::DEFAULT_VERT], SHADER_PATHS[ShaderIndices::DEFAULT_FRAG], defaultMeshRendererId);
        // Cubemap renderer
        std::string cubemapMeshRendererId = "Cubemap renderer";
        registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::CUBEMAP_VERT], SHADER_PATHS[ShaderIndices::CUBEMAP_FRAG], cubemapMeshRendererId);
    }

    Entity Shape::createCubeEntity(Scene* scene, const TextureAttachmentSpecification& textureAtttachments) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::Cube);
        std::string meshRendererId = "Default renderer";
        auto meshRendererSpecification = meshRendererRegistry->get(meshRendererId);
        if(!meshRendererSpecification)
            meshRendererSpecification = registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::DEFAULT_VERT], SHADER_PATHS[ShaderIndices::DEFAULT_FRAG], meshRendererId);
        std::string meshId = "Cube mesh";
        auto meshSpecification = meshRegistry->get(meshId);
        if(meshSpecification)
            entity.addComponent<MeshComponent>(meshSpecification->vao, meshSpecification->vbo, meshSpecification->ebo, meshId, textureAtttachments);
        else {
            auto& meshComponent = entity.addComponent<MeshComponent>(Shape::cubeVertices, sizeof(Shape::cubeVertices), 
                Shape::cubeIndices, sizeof(Shape::cubeIndices), meshId, textureAtttachments);
            registerMeshSpecification(meshComponent.vao, meshComponent.vbo, meshComponent.ebo, EntityCreationType::Cube, "", meshId);
        }
        auto& meshRendererComponent = entity.addComponent<MeshRendererComponent>(meshRendererSpecification->vertexShaderPath.c_str(), 
            meshRendererSpecification->fragmentShaderPath.c_str(), (int)entity.getSource(), meshRendererId);
        auto& transformComponent = entity.getComponent<TransformComponent>();
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Cube");
        return entity;
    }

    Entity Shape::createPlateEntity(Scene* scene, const TextureAttachmentSpecification& textureAtttachments) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::Plate);
        std::string meshRendererId = "Default renderer";
        auto meshRendererSpecification = meshRendererRegistry->get(meshRendererId);
        if(!meshRendererSpecification)
            meshRendererSpecification = registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::DEFAULT_VERT], SHADER_PATHS[ShaderIndices::DEFAULT_FRAG], meshRendererId);
        std::string meshId = "Plate mesh";
        auto meshSpecification = meshRegistry->get(meshId);
        if(meshSpecification)
            entity.addComponent<MeshComponent>(meshSpecification->vao, meshSpecification->vbo, meshSpecification->ebo, meshId, textureAtttachments);
        else {
            auto& meshComponent = entity.addComponent<MeshComponent>(Shape::plateVertices, sizeof(Shape::plateVertices), 
                Shape::plateIndices, sizeof(Shape::plateIndices), meshId, textureAtttachments);
            registerMeshSpecification(meshComponent.vao, meshComponent.vbo, meshComponent.ebo, EntityCreationType::Plate, "", meshId);
        }
        auto& meshRendererComponent = entity.addComponent<MeshRendererComponent>(meshRendererSpecification->vertexShaderPath.c_str(), 
            meshRendererSpecification->fragmentShaderPath.c_str(), (int)entity.getSource(), meshRendererId);
        auto& transformComponent = entity.getComponent<TransformComponent>();
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Plate");
        return entity;
    }

    Entity Shape::createCubemapEntity(Scene* scene, TextureAttachmentSpecification& textureAtttachments) {
        Texture* texture = Texture::generateCubemapTexture(textureAtttachments);
        if(!texture)
            return {};
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::Cubemap);
        std::string meshRendererId = "Cubemap renderer";
        auto meshRendererSpecification = meshRendererRegistry->get(meshRendererId);
        if(!meshRendererSpecification)
            meshRendererSpecification = registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::CUBEMAP_VERT], SHADER_PATHS[ShaderIndices::CUBEMAP_FRAG], meshRendererId);
        auto& transformComponent = entity.getComponent<TransformComponent>();
        std::string meshId = "Cubemap mesh";
        auto meshSpecification = meshRegistry->get(meshId);
        if(meshSpecification)
            entity.addComponent<MeshComponent>(meshSpecification->vao, meshSpecification->vbo, meshSpecification->ebo, meshId, texture);
        else {
            auto& meshComponent = entity.addComponent<MeshComponent>(Shape::cubeVertices, sizeof(Shape::cubeVertices), 
                Shape::cubemapIndices, sizeof(Shape::cubemapIndices), meshId, texture);
            registerMeshSpecification(meshComponent.vao, meshComponent.vbo, meshComponent.ebo, EntityCreationType::Cubemap, "", meshId);
        }
        auto& meshRendererComponent = entity.addComponent<MeshRendererComponent>(meshRendererSpecification->vertexShaderPath.c_str(), 
            meshRendererSpecification->fragmentShaderPath.c_str(), (int)entity.getSource(), meshRendererId);
        transformComponent.scale({1000.f, 1000.f, 1000.f});
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Cubemap");
        return entity;
    }

    Entity Shape::createSpotLightEntity(Scene* scene, const glm::vec3& color, float innerRadius, float outerRadius, float constant, float linear, float quadratic) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::SpotLight);
        entity.addComponent<LightComponent>(color, innerRadius, outerRadius, constant, linear, quadratic, LightType::Spot);
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Spot light");
        return entity;
    }

    Entity Shape::createPointLightEntity(Scene* scene, const glm::vec3& color, float constant, float linear, float quadratic) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::PointLight);
        entity.addComponent<LightComponent>(color, 15.f, 20.f, constant, linear, quadratic, LightType::Point);
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Point light");
        return entity;
    }

    Entity Shape::createDirLightEntity(Scene* scene, const glm::vec3& color) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::DirLight);
        entity.addComponent<LightComponent>(color, 15.f, 20.f, 1.f, 0.045f, 0.0075f, LightType::Dir);
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Dir light");
        return entity;
    }

    Entity Shape::createCameraEntity(Scene* scene) {
        Entity entity = scene->createEntity();
        auto& creationType = entity.getComponent<CreationTypeComponent>();
        creationType.setType(EntityCreationType::Camera);
        entity.addComponent<CameraComponent>();
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName("Camera");
        return entity;
    }

    std::vector<Entity> Shape::createModelEntity(Scene* scene, ModelLoaderData* modelLoaderData) {
        std::vector<Entity> models;
        entt::registry* registry = scene->getRegistry();
        for(auto& mesh : modelLoaderData->meshComponents){
            Entity entity = scene->createEntity();
            auto& creationType = entity.getComponent<CreationTypeComponent>();
            creationType.setType(EntityCreationType::Model);
            std::string meshRendererId = "Default renderer";
            auto meshRendererSpecification = meshRendererRegistry->get(meshRendererId);
            if(!meshRendererSpecification)
                meshRendererSpecification = registerMeshRendererSpecification(SHADER_PATHS[ShaderIndices::DEFAULT_VERT], SHADER_PATHS[ShaderIndices::DEFAULT_FRAG], meshRendererId);
            auto meshSpecs = meshRegistry->getValues();
            std::string meshId;
            for(auto& spec : meshSpecs)
                if(std::filesystem::absolute(spec->modelPath) == std::filesystem::absolute(modelLoaderData->fullPath)) {
                    meshId = spec->meshId;
                    break;
                }
            if(meshId.empty())
                meshId = "Model mesh-" + std::to_string(meshCounter++);
            auto meshSpecification = meshRegistry->get(meshId);
            if(meshSpecification)
                entity.addComponent<MeshComponent>(meshSpecification->vao, meshSpecification->vbo, meshSpecification->ebo, meshId);
            else {
                mesh.registryId = meshId;
                auto& meshComponent = entity.addComponent<MeshComponent>(mesh);
                registerMeshSpecification(meshComponent.vao, meshComponent.vbo, meshComponent.ebo, EntityCreationType::Model, modelLoaderData->fullPath, meshId);
            }
            auto& meshRendererComponent = entity.addComponent<MeshRendererComponent>(SHADER_PATHS[modelLoaderData->vert], 
                SHADER_PATHS[modelLoaderData->frag], (int)entity.getSource(), meshRendererId);
            auto& nameComponent = entity.getComponent<NameComponent>();
            nameComponent.setName("Model");
            models.push_back(entity);
            auto& meshComponent = entity.getComponent<MeshComponent>();
            meshComponent.modelPath = modelLoaderData->fullPath;
        }
        return models;
    }

    MeshSpecification* Shape::registerMeshSpecification(std::shared_ptr<VAO> vao, std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo, 
    EntityCreationType creationType, const std::string& modelPath, const std::string& id) {
        auto meshSpecification = meshRegistry->add<MeshSpecification>(id);
        meshSpecification->vao = vao;
        meshSpecification->vbo = vbo;
        meshSpecification->ebo = ebo;
        meshSpecification->creationType = creationType;
        meshSpecification->modelPath = modelPath;
        meshSpecification->meshId = id;
        return meshSpecification;
    }

    MeshRendererSpecification* Shape::registerMeshRendererSpecification(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& id) {
        auto meshRendererSpecification = meshRendererRegistry->add<MeshRendererSpecification>(id);
        meshRendererSpecification->vertexShaderPath = vertexShaderPath;
        meshRendererSpecification->fragmentShaderPath = fragmentShaderPath;
        return meshRendererSpecification;
    }
}