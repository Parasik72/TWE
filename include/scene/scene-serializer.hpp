#ifndef SCENE_SERIALIZER_HPP
#define SCENE_SERIALIZER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <single_include/nlohmann/json.hpp>

#include "scene/iscene.hpp"
#include "scene/shape.hpp"
#include "stream/file.hpp"
#include "stream/dll-creator.hpp"
#include "stream/project-creator.hpp"
#include "model-loader/model-loader.hpp"
#include "registry/registry.hpp"

namespace TWE {
    class SceneSerializer {
    public:
        static bool serialize(IScene* scene, const std::string& path, ProjectData* projectData);
        static bool deserialize(IScene* scene, const std::string& path, ProjectData* projectData);
    private:
        static void serializeEntity(Entity& entity, nlohmann::json& jsonEntities, ProjectData* projectData, IScene* scene);
        static void serializeCreationTypeComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeNameComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeTransformComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeMeshComponent(Entity& entity, nlohmann::json& jsonEntity, ProjectData* projectData);
        static void serializeMeshRendererComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeCameraComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeLightComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializePhysicsComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeScriptComponent(Entity& entity, nlohmann::json& jsonEntity);
        static void serializeParentChildsComponent(Entity& entity, nlohmann::json& jsonEntity, IScene* scene);
        static void serializeAudioComponent(Entity& entity, nlohmann::json& jsonEntity, ProjectData* projectData);

        static void deserializeEntity(IScene* scene, Entity& entity, nlohmann::json& jsonComponents, const std::filesystem::path& rootPath);
        static void deserializeNameComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeTransformComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeMeshRendererComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeCameraComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeLightComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializePhysicsComponent(IScene* scene, Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeScriptComponent(IScene* scene, Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeParentChildsComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeIDComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent);
        static void deserializeAudioComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent, const std::filesystem::path& rootPath);
        [[nodiscard]] static Entity deserializeCreationTypeComponent(IScene* scene, nlohmann::json& jsonComponent, const std::filesystem::path& rootPath);

        static void revalidateParentChildsComponent(IScene* scene);

        [[nodiscard]] static std::string deleteInvertedCommas(const std::string& str);
    };
};

#endif