#include "scene/scene-physics.hpp"

namespace TWE {
    ScenePhysics::ScenePhysics() {
        _collisionConfig = new btDefaultCollisionConfiguration();
        _dispatcher = new btCollisionDispatcher(_collisionConfig);
        _broadPhase = new btDbvtBroadphase();
        _solver = new btSequentialImpulseConstraintSolver();
        _world = new btDiscreteDynamicsWorld(_dispatcher, _broadPhase, _solver, _collisionConfig); 
        _world->setGravity({0.f, -9.81f, 0.f});
        _debugDrawer = new SceneBulletDebugDrawer();
        _debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
        _world->setDebugDrawer(_debugDrawer);
    }

    void ScenePhysics::debugDrawWorld() {
        _world->debugDrawWorld();
    }

    void ScenePhysics::checkCollisionsDetection() {
        int numManifolds = _world->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = _world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();
            int numContacts = contactManifold->getNumContacts();
            for (int j = 0; j < numContacts; j++) {
                btManifoldPoint& manifoldPoint = contactManifold->getContactPoint(j);
                if(manifoldPoint.getDistance() < 0.f) {
                    if (std::find(collisions[obA->getUserIndex()].begin(), collisions[obA->getUserIndex()].end(), 
                    obB) == collisions[obA->getUserIndex()].end())
                        collisions[obA->getUserIndex()].emplace_back(obB);
                }
            }
        }
    }

    std::vector<const btCollisionObject*> ScenePhysics::getCollisionDetection(const btCollisionObject* obj) {
        auto item = collisions.find(obj->getUserIndex());
        if(item == collisions.end())
            return {};
        return item->second;
    }

    void ScenePhysics::cleanCollisionDetection() {
        collisions.clear();
    }
    
    btDynamicsWorld* ScenePhysics::getDynamicWorld() const noexcept { return _world; }
    SceneBulletDebugDrawer* ScenePhysics::getDebugDrawer() const noexcept { return _debugDrawer; }
}