#include "entity/entity.hpp"

namespace TWE {
    Entity::Entity(): _entity(entt::null), _scene(nullptr) {}

    Entity::Entity(entt::entity entity, TWE::Scene* scene): _entity(entity), _scene(scene) {}

    Entity::Entity(const Entity& entity) {
        this->_entity = entity._entity;
        this->_scene = entity._scene;
    }

    bool Entity::operator==(const Entity& other) {
        return this->_entity == other._entity && this->_scene == other._scene;
    }

    bool Entity::operator!=(const Entity& other) {
        return !(*this == other);
    }

    Scene* Entity::getScene() { return _scene; }
    entt::entity Entity::getSource() const noexcept { return _entity; }
}