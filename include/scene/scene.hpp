#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <memory>
#include <algorithm>
#include <iterator>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <entt/entt.hpp>

#include "renderer/debug-camera.hpp"
#include "renderer/camera.hpp"
#include "time.hpp"
#include "renderer/renderer.hpp"
#include "entity/entity.hpp"

namespace TWE {
    class Scene {
    public:
        Scene();
        ~Scene();
        void update();
        void draw();
        void setTransMat(const glm::mat4& transform, TransformMatrixOptions option);
        void setLight(const LightComponent& light, const TransformComponent& transform, const MeshRendererComponent& meshRenderer, const  uint32_t index);
        void setViewPos(const glm::vec3& pos);
        void updateView(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& pos);
        void generateShadows(uint32_t windowWidth, uint32_t windowHeight);
        void updatePhysics();
        void linkRigidBody(const PhysicsComponent& physicsComponent);
        void setFocusOnDebugCamera(bool isFocusedOnDebugCamera);
        void setDrawLightMeshes(bool drawLightMeshes);
        void setDebugCamera(DebugCamera* debugCamera);
        Entity createEntity();
        [[nodiscard]] bool& getIsFocusedOnDebugCamera();
        [[nodiscard]] bool getIsFocusedOnDebugCamera() const noexcept;
        [[nodiscard]] bool getDrawLightMeshes() const noexcept;
        [[nodiscard]] entt::registry* getRegistry() const noexcept;
        [[nodiscard]] btDynamicsWorld* getDynamicWorld() const noexcept;
    private:
        bool updateView();
        void generateDepthMap(LightComponent& lightComponent, const TransformComponent& transformComponent, const glm::mat4& lightProjection, const glm::mat4& lightView);
        void setShadows(const LightComponent& lightComponent, const glm::mat4& lightSpaceMat, int index);
        GLint _lightsCount;
        std::unique_ptr<btDynamicsWorld> _world;
        std::unique_ptr<btDispatcher> _dispatcher;
        std::unique_ptr<btConstraintSolver> _solver;
        std::unique_ptr<btCollisionConfiguration> _collisionConfig;
        std::unique_ptr<btBroadphaseInterface> _broadPhase;
        std::unique_ptr<entt::registry> _registry;
        DebugCamera* _debugCamera;
        bool _isFocusedOnDebugCamera;
        bool _drawLightMeshes;
        friend class Entity;
    };
}

#endif