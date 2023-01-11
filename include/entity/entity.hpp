#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "entt/entt.hpp"

namespace TWE {
    class Scene;
    class Entity {
    public:
        Entity();
        Entity(entt::entity entity, Scene* scene);
        Entity(const Entity& entity);
        template<typename T>
        bool hasComponent();

        template<typename T>
        T& getComponent();

        template<typename T, typename ...Args>
        T& addComponent(Args&&... args);

        template<typename T>
        void removeComponent();

        template<typename T = void>
        void destroy();

        bool operator==(const Entity& other);
        bool operator!=(const Entity& other);
        [[nodiscard]] Scene* getScene();
        [[nodiscard]] entt::entity getSource() const noexcept;
    private:
        entt::entity _entity;
        Scene* _scene;
    };

    template<typename T>
    bool Entity::hasComponent() {
        return _scene->_sceneRegistry.current->entityRegistry.any_of<T>(_entity);
    }

    template<typename T>
    T& Entity::getComponent() {
        return _scene->_sceneRegistry.current->entityRegistry.get<T>(_entity);
    }

    template<typename T, typename ...Args>
    T& Entity::addComponent(Args&&... args) {
        return _scene->_sceneRegistry.current->entityRegistry.emplace<T>(_entity, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::removeComponent() {
        _scene->_sceneRegistry.current->entityRegistry.remove<T>(_entity);
    }

    template<typename T>
    void Entity::destroy() {
        _scene->cleanEntity(*this);
        _entity = entt::null;
        _scene = nullptr;
    }
}

#endif