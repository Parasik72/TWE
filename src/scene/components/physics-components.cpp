#include "scene/components/physics-components.hpp"

namespace TWE {
    PhysicsComponent::PhysicsComponent(): _colliderType(ColliderType::None), _needUpdate(true) {}

    PhysicsComponent::PhysicsComponent(btDynamicsWorld* dynamicsWorld, ColliderType colliderType, const glm::vec3& shapeSize, 
    const glm::vec3& localScale, const glm::vec3& pos, const glm::vec3& rotation, float mass, entt::entity entity)
    : _colliderType(colliderType), _dynamicsWorld(dynamicsWorld), _needUpdate(true) {
        btCollisionShape* shape = createShape(colliderType, shapeSize);
        if(!shape)
            return;
        _transform.setIdentity();
        _transform.setOrigin({pos.x, pos.y, pos.z});

        btVector3 inertia(0.f, 0.f, 0.f);
        shape->setLocalScaling({localScale.x, localScale.y, localScale.z});
        shape->calculateLocalInertia(mass, inertia);

        btMotionState* motionState = new btDefaultMotionState(_transform);
        btRigidBody::btRigidBodyConstructionInfo constrInfo(mass, motionState, shape, inertia);

        _rigidBody = new btRigidBody(constrInfo);
        _rigidBody->setUserPointer(this);
        int collisionFlags = _rigidBody->getCollisionFlags();
        _rigidBody->setCollisionFlags(collisionFlags ^ btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT ^ btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        
        dynamicsWorld->addRigidBody(_rigidBody);
        setRotation(rotation);
        _rigidBody->setUserPointer(new PhysicsUserPointer{entity});
    }

    PhysicsComponent::PhysicsComponent(btDynamicsWorld* dynamicsWorld, ColliderType colliderType, TriangleMeshSpecification& triangleMeshSpecification, 
    const glm::vec3& localScale, const glm::vec3& pos, const glm::vec3& rotation, entt::entity entity)
    : _colliderType(colliderType), _dynamicsWorld(dynamicsWorld), _triangleMesh(triangleMeshSpecification) {
        btCollisionShape* shape = createShape(colliderType, triangleMeshSpecification);
        if(!shape)
            return;
        btVector3 inertia(0.f, 0.f, 0.f);
        shape->setLocalScaling({localScale.x, localScale.y, localScale.z});

        btMotionState* motionState = new btDefaultMotionState();
        btRigidBody::btRigidBodyConstructionInfo constrInfo(0.f, motionState, shape, inertia);

        _rigidBody = new btRigidBody(constrInfo);
        _rigidBody->setUserPointer(this);
        int collisionFlags = _rigidBody->getCollisionFlags();
        _rigidBody->setCollisionFlags(collisionFlags ^ btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        
        dynamicsWorld->addRigidBody(_rigidBody);
        setPosition(pos);
        setRotation(rotation);
        _rigidBody->setUserPointer(new PhysicsUserPointer{entity});
    }

    PhysicsComponent::PhysicsComponent(const PhysicsComponent& physics) {
        this->_rigidBody = physics._rigidBody;
        this->_colliderType = physics._colliderType;
        this->_dynamicsWorld = physics._dynamicsWorld;
        this->_transform = physics._transform;
        this->_triangleMesh = physics._triangleMesh;
        this->_needUpdate = physics._needUpdate;
    }

    void PhysicsComponent::setMass(float mass) {
        _dynamicsWorld->removeRigidBody(_rigidBody);
        btVector3 inertia(0.f, 0.f, 0.f);
        if(_colliderType != ColliderType::TriangleMesh)
            _rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
        _rigidBody->setMassProps(mass, inertia);
        _dynamicsWorld->addRigidBody(_rigidBody);
    }

    void PhysicsComponent::setPosition(const glm::vec3& pos) {
        btTransform oldTransform = _rigidBody->getWorldTransform();
        btQuaternion oldBodyRotation = oldTransform.getRotation();
        _transform.setIdentity();
        _transform.setRotation(oldBodyRotation);
        _transform.setOrigin({pos.x, pos.y, pos.z});
        _rigidBody->setWorldTransform(_transform);
    }

    void PhysicsComponent::setRotation(const glm::vec3& rotation) {
        _transform = _rigidBody->getWorldTransform();
        btQuaternion newRotation(rotation.y, rotation.x, rotation.z);
        btQuaternion oldBodyRotation = _transform.getRotation();
        btQuaternion deltaRotation = newRotation - oldBodyRotation;
        _transform.setRotation(oldBodyRotation + deltaRotation);
        _rigidBody->setWorldTransform(_transform);
    }

    void PhysicsComponent::setRotation(const glm::quat& quatretion) {
        _transform = _rigidBody->getWorldTransform();
        _transform.setRotation({quatretion.x, quatretion.y, quatretion.z, quatretion.w});
        _rigidBody->setWorldTransform(_transform);
    }

    void PhysicsComponent::setSize(const glm::vec3& size) {
        _dynamicsWorld->removeRigidBody(_rigidBody);
        btCollisionShape* shape = _rigidBody->getCollisionShape();
        shape->setLocalScaling({size.x, size.y, size.z});
        _dynamicsWorld->addRigidBody(_rigidBody);
    }

    void PhysicsComponent::setColliderType(ColliderType type) {
        btCollisionShape* collisionShape = createShape(type, glm::vec3(1.f));
        if(collisionShape) {
            collisionShape->setMargin(0.f);
            _rigidBody->setCollisionShape(collisionShape);
            _colliderType = type;
        }
    }

    void PhysicsComponent::setColliderType(ColliderType type, TriangleMeshSpecification& triangleMeshSpecification) {
        btCollisionShape* collisionShape = createShape(type, triangleMeshSpecification);
        if(collisionShape) {
            collisionShape->setMargin(0.f);
            _rigidBody->setCollisionShape(collisionShape);
            _triangleMesh = triangleMeshSpecification;
            _colliderType = type;
            setMass(0.f);
        }
    }

    void PhysicsComponent::setShapeDimensions(const glm::vec3& size) {
        _dynamicsWorld->removeRigidBody(_rigidBody);
        btVector3 shapeDimensions = { size.x, size.y, size.z };
        switch (_colliderType) {
        case ColliderType::Box: 
        case ColliderType::Sphere:
        case ColliderType::Cylinder:
        case ColliderType::Capsule:
            btBoxShape* boxShape; 
            boxShape = (btBoxShape*)_rigidBody->getCollisionShape();
            boxShape->setImplicitShapeDimensions(shapeDimensions / 2);
            break;
        case ColliderType::Cone:
            btConeShape* coneShape;
            coneShape = (btConeShape*)createShape(ColliderType::Cone, {size.x, size.y, size.z});
            _rigidBody->setCollisionShape(coneShape);
        }
        _dynamicsWorld->addRigidBody(_rigidBody);
    }

    glm::vec3 PhysicsComponent::getShapeDimensions() { 
        btVector3 shapeDimensions;
        btCollisionShape* collisionShape = _rigidBody->getCollisionShape();
        collisionShape->setMargin(0.f);
        btBoxShape* boxShape = boxShape = (btBoxShape*)collisionShape;
        shapeDimensions = boxShape->getImplicitShapeDimensions();
        btVector3& size = (shapeDimensions * 2);
        return { size.x(), size.y(), size.z() };
    }
    
    glm::vec3 PhysicsComponent::getLocalScale() { 
        auto& localScaling = _rigidBody->getCollisionShape()->getLocalScaling(); 
        return { localScaling.x(), localScaling.y(), localScaling.z() };
    }

    glm::vec3 PhysicsComponent::getPosition() {
        auto& position = _rigidBody->getWorldTransform().getOrigin();
        return { position.x(), position.y(), position.z() };
    }

    glm::vec3 PhysicsComponent::getRotation() {
        auto& quat = _rigidBody->getWorldTransform().getRotation();
        glm::vec3 rot;
        quat.getEulerZYX(rot.z, rot.y, rot.x);
        return rot;
    }

    btCollisionShape* PhysicsComponent::createShape(ColliderType colliderType, const glm::vec3& shapeSize) {
        btVector3 halfSize = { shapeSize.x / 2.f, shapeSize.y / 2.f, shapeSize.z / 2.f };
        switch (colliderType) {
        case ColliderType::Box:
            return new btBoxShape(halfSize);
        case ColliderType::Sphere:
            return new btSphereShape(halfSize.x());
        case ColliderType::Cylinder:
            return new btCylinderShape(halfSize);
        case ColliderType::Cone:
            return new btConeShape(halfSize.x(), halfSize.y());
        case ColliderType::Capsule:
            return new btCapsuleShape(halfSize.x(), halfSize.y());
        default:
            return nullptr;
        }
    }

    btCollisionShape* PhysicsComponent::createShape(ColliderType colliderType, TriangleMeshSpecification& triangleMeshSpecification) {
        btTriangleIndexVertexArray* triangleMesh;
        int numTriangles = triangleMeshSpecification.ebo->getSize() / sizeof(int) / 3;
        if(numTriangles >= 100000)
            return nullptr;
        int* triangleIndexBase = (int*)triangleMeshSpecification.ebo->getIndices();
        int triangleIndexStride = 3 * sizeof(int);
        int numVertices = triangleMeshSpecification.vbo->getSize() / sizeof(float) / 8;
        btScalar* vertexBase = triangleMeshSpecification.vbo->getVertices();
        int vertexStride = 8 * sizeof(float);
        triangleMesh = new btTriangleIndexVertexArray(numTriangles, triangleIndexBase, triangleIndexStride, numVertices, vertexBase, vertexStride);
        return new btBvhTriangleMeshShape(triangleMesh, false);
    }

    void PhysicsComponent::setNeedUpdate(bool needUpdate) {
        _needUpdate = needUpdate;
    }

    void PhysicsComponent::setIsRotated(bool isRotated) {
        _rigidBody->setAngularFactor(isRotated ? 1.f : 0.f);
    }

    bool PhysicsComponent::getIsRotated() {
        return _rigidBody->getAngularFactor() == btVector3{1.f, 1.f, 1.f};
    }

    void PhysicsComponent::setShowCollider(bool show) {
        int collisionFlags = _rigidBody->getCollisionFlags();
        if(show && (collisionFlags & btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT))
            _rigidBody->setCollisionFlags(collisionFlags ^ btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        else if(!show && !(collisionFlags & btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT))
            _rigidBody->setCollisionFlags(collisionFlags ^ btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }

    void PhysicsComponent::setRigidBody(btRigidBody* rigidBody) {
        _rigidBody = rigidBody;
    }

    btRigidBody* PhysicsComponent::getRigidBody() const noexcept { return _rigidBody; }
    btDynamicsWorld* PhysicsComponent::getDynamicsWorld() const noexcept { return _dynamicsWorld; }
    ColliderType PhysicsComponent::getColliderType() const noexcept { return _colliderType; }
    float PhysicsComponent::getMass() const noexcept { return _rigidBody->getMass(); }
    bool PhysicsComponent::getNeedUpdate() const noexcept { return _needUpdate; }
    TriangleMeshSpecification& PhysicsComponent::getTriangleMesh() { return _triangleMesh; }

    std::vector<std::string> colliderTypes = {
        "Box",
        "Sphere",
        "Cylinder",
        "Cone",
        "Capsule",
        "TriangleMesh"
    };
}