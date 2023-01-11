#ifndef PHYSICS_COMPONENT_HPP
#define PHYSICS_COMPONENT_HPP

#include <glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "entt/entt.hpp"

#include "twe-math/twe-math.hpp"
#include "renderer/vao.hpp"
#include "renderer/vbo.hpp"
#include "renderer/ebo.hpp"

namespace TWE {
    enum class ColliderType {
        None = -1,
        Box,
        Sphere,
        Cylinder,
        Cone,
        Capsule,
        TriangleMesh
    };

    extern std::vector<std::string> colliderTypes;

    struct TriangleMeshSpecification {
        TriangleMeshSpecification() = default;
        TriangleMeshSpecification(std::shared_ptr<VBO> vbo, std::shared_ptr<EBO> ebo)
            : vbo(vbo), ebo(ebo) {}
        TriangleMeshSpecification(const TriangleMeshSpecification& triangleMeshSpecification) {
            this->vbo = triangleMeshSpecification.vbo;
            this->ebo = triangleMeshSpecification.ebo;
        }
        std::shared_ptr<VBO> vbo = nullptr;
        std::shared_ptr<EBO> ebo = nullptr;
    };

    struct PhysicsUserPointer {
        PhysicsUserPointer(entt::entity entity): entity(entity){}
        entt::entity entity;
    };

    class PhysicsComponent {
    public:
        PhysicsComponent();
        PhysicsComponent(btDynamicsWorld* dynamicsWorld, ColliderType colliderType, const glm::vec3& shapeSize, const glm::vec3& localScale, const glm::vec3& pos, 
            const glm::vec3& rotation, float mass, entt::entity entity);
        PhysicsComponent(btDynamicsWorld* dynamicsWorld, ColliderType colliderType, TriangleMeshSpecification& triangleMeshSpecification, 
            const glm::vec3& localScale, const glm::vec3& pos, const glm::vec3& rotation, entt::entity entity);
        PhysicsComponent(const PhysicsComponent& physics);
        void setMass(float mass);
        void setPosition(const glm::vec3& pos);
        void setRotation(const glm::vec3& rotation);
        void setRotation(const glm::quat& quatretion);
        void setSize(const glm::vec3& size);
        void setColliderType(ColliderType type);
        void setColliderType(ColliderType type, TriangleMeshSpecification& triangleMeshSpecification);
        void setShapeDimensions(const glm::vec3& size);
        void setNeedUpdate(bool needUpdate);
        void setIsRotated(bool isRotated);
        void setShowCollider(bool show);
        void setRigidBody(btRigidBody* rigidBody);
        [[nodiscard]] float getMass() const noexcept;
        [[nodiscard]] btRigidBody* getRigidBody() const noexcept;
        [[nodiscard]] btDynamicsWorld* getDynamicsWorld() const noexcept;
        [[nodiscard]] ColliderType getColliderType() const noexcept;
        [[nodiscard]] bool getNeedUpdate() const noexcept;
        [[nodiscard]] bool getIsRotated();
        [[nodiscard]] glm::vec3 getShapeDimensions();
        [[nodiscard]] glm::vec3 getLocalScale();
        [[nodiscard]] glm::vec3 getPosition();
        [[nodiscard]] glm::vec3 getRotation();
        [[nodiscard]] TriangleMeshSpecification& getTriangleMesh();
        static btCollisionShape* createShape(ColliderType colliderType, const glm::vec3& shapeSize);
        static btCollisionShape* createShape(ColliderType colliderType, TriangleMeshSpecification& triangleMeshSpecification);
    private:
        bool _needUpdate;
        btRigidBody* _rigidBody;
        ColliderType _colliderType;
        btTransform _transform;
        btDynamicsWorld* _dynamicsWorld;
        TriangleMeshSpecification _triangleMesh;
    };
}

#endif