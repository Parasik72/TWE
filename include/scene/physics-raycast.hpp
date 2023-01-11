#ifndef PHYSICS_RAYCAST_HPP
#define PHYSICS_RAYCAST_HPP

#include <glm.hpp>

#include "entity/entity.hpp"
#include "scene/components/physics-components.hpp"

namespace TWE {
    struct RaycastClosestInfo {
        RaycastClosestInfo() = default;
        RaycastClosestInfo(bool isHit, Entity& entity, const btCollisionObject* collisionObject)
            : isHit(isHit), entity(entity), collisionObject(collisionObject) {}
        bool isHit = false;
        Entity entity;
        const btCollisionObject* collisionObject;
    };

    struct RaycastAllHitsInfo {
        RaycastAllHitsInfo() = default;
        RaycastAllHitsInfo(bool isHit, const std::vector<Entity>& entities, const std::vector<const btCollisionObject*>& collisionObjects)
            : isHit(isHit), entities(entities), collisionObjects(collisionObjects) {}
        bool isHit = false;
        std::vector<Entity> entities;
        std::vector<const btCollisionObject*> collisionObjects;
    };

    class PhysicsRaycast {
    public:
        template<typename T = void>
        static RaycastClosestInfo raycastClosest(Scene* scene, const glm::vec3& from, const glm::vec3& to);
        template<typename T = void>
        static RaycastAllHitsInfo raycastAllHits(Scene* scene, const glm::vec3& from, const glm::vec3& to);
    };

    template<typename T>
    RaycastClosestInfo PhysicsRaycast::raycastClosest(Scene* scene, const glm::vec3& from, const glm::vec3& to) {
        btVector3 rayFromWorld = {from.x, from.y, from.z};
        btVector3 rayToWorld = {to.x, to.y, to.z};
        btCollisionWorld::ClosestRayResultCallback rayResultCallback(rayFromWorld, rayToWorld);
        scene->getDynamicWorld()->rayTest(rayFromWorld, rayToWorld, rayResultCallback);
        if(!rayResultCallback.hasHit())
            return {};
        auto userPointer = (PhysicsUserPointer*)rayResultCallback.m_collisionObject->getUserPointer();
        return { true, Entity{ userPointer->entity, scene }, rayResultCallback.m_collisionObject};
    }

    template<typename T>
    RaycastAllHitsInfo PhysicsRaycast::raycastAllHits(Scene* scene, const glm::vec3& from, const glm::vec3& to) {
        btVector3 rayFromWorld = {from.x, from.y, from.z};
        btVector3 rayToWorld = {to.x, to.y, to.z};
        btCollisionWorld::AllHitsRayResultCallback rayResultCallback(rayFromWorld, rayToWorld);
        scene->getDynamicWorld()->rayTest(rayFromWorld, rayToWorld, rayResultCallback);
        if(!rayResultCallback.hasHit())
            return {};
        std::vector<Entity> entities;
        std::vector<const btCollisionObject*> collisionObjects;
        int size = rayResultCallback.m_collisionObjects.size();
        for(int i = 0; i < size; ++i) {
            auto userPointer = (PhysicsUserPointer*)rayResultCallback.m_collisionObjects[i]->getUserPointer();
            entities.push_back(Entity{ userPointer->entity, scene });
            collisionObjects.push_back(rayResultCallback.m_collisionObjects[i]);
        }
        return { true, entities, collisionObjects };
    }
}

#endif