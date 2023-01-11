#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <entt/entt.hpp>

#include "renderer/renderer.hpp"
#include "renderer/debug-camera.hpp"
#include "renderer/camera.hpp"

#include "time.hpp"
#include "input/input.hpp"
#include "registry/registry.hpp"
#include "stream/dll-creator.hpp"
#include "undo-redo/ur-control.hpp"

#include "scene/scene-bullet-debug-drawer.hpp"
#include "scene/scene-physics.hpp"
#include "scene/physics-raycast.hpp"
#include "scene/shape.hpp"

namespace TWE {
    enum class SceneState {
        Edit,
        Run,
        Pause
    };

    struct SceneStateSpecification {
        entt::registry entityRegistry;
        ScenePhysics physics;
        URControl urControl;
        int lastId = 0;
    };

    struct SceneRegistrySpecification {
        SceneStateSpecification* current = nullptr;
        SceneStateSpecification edit;
        SceneStateSpecification run;
    };

    struct SceneCameraSpecification {
        Camera* camera = nullptr;
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 forward = glm::vec3(0.f);
        glm::vec3 up = glm::vec3(0.f);
    };

    class Scene {
    public:
        Scene(uint32_t windowWidth, uint32_t windowHeight);
        void update();
        void draw();
        void reset();
        void updateView(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& pos);
        void validateScripts();
        void validateScript(const std::string& scriptName);
        void setDebugCamera(DebugCamera* debugCamera);
        void setName(const std::string& name);
        void setScriptDLLRegistry(Registry<DLLLoadData>* scriptDLLRegistry);
        void setState(SceneState state);
        void cleanEntity(Entity& entity);
        void bindScript(DLLLoadData* dllData, Entity& entity);
        void bindScript(DLLLoadData* dllData, std::vector<Entity>& entities);
        std::vector<Entity> unbindScript(entt::registry* registry, DLLLoadData* dllData);
        Entity createEntity(const std::string& name = "Entity");
        Entity copyEntityState(Entity& entity, SceneStateSpecification& to);
        [[nodiscard]] bool& getIsFocusedOnDebugCamera();
        [[nodiscard]] entt::registry* getRegistry() const noexcept;
        [[nodiscard]] btDynamicsWorld* getDynamicWorld() const noexcept;
        [[nodiscard]] std::string getName() const noexcept;
        [[nodiscard]] FBO* getFrameBuffer() const noexcept;
        [[nodiscard]] Registry<DLLLoadData>* getScriptDLLRegistry() const noexcept;
        [[nodiscard]] SceneStateSpecification* getSceneStateSpecification();
    private:
        void updateEditState();
        void updateTransforms();
        void updateChildsTransform(Entity& entity, ModelSpecification& ratioTransform, const glm::vec3& centerPositon);
        void updateRunState();
        void updatePhysics();
        void updateScripts();
        void updateLight();
        bool updateView();
        void resetEntityRegistry(entt::registry* registry);
        void resetScripts(entt::registry* registry);
        void resetPhysics(entt::registry* registry);
        void updateShadows(uint32_t windowWidth, uint32_t windowHeight);
        void setShadows(const LightComponent& lightComponent, const glm::mat4& lightSpaceMat, int index);
        void setTransMat(const glm::mat4& transform, TransformMatrixOptions option);
        void setLight(const LightComponent& light, TransformComponent& transform, const  uint32_t index);
        void setViewPos(const glm::vec3& pos);
        void copySceneState(SceneStateSpecification& from, SceneStateSpecification& to);

        SceneState _sceneState;
        SceneRegistrySpecification _sceneRegistry;
        std::unique_ptr<FBO> _frameBuffer;
        DebugCamera* _debugCamera;
        SceneCameraSpecification _sceneCameraSpecification;
        bool _isFocusedOnDebugCamera;
        bool _drawColliders;
        std::string _name;
        Registry<DLLLoadData>* _scriptDLLRegistry;

        friend class Entity;
        friend class GUI;
        friend class GUIComponentsPanel;
        friend class GUIDirectoryPanel;
        friend class GUIScenePanel;
        friend class SceneSerializer;
    };
}

#endif