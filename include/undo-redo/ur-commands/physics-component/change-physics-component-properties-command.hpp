#ifndef CHANGE_PHYSICS_COMPONENT_PROPERTIES_COMMAND_HPP
#define CHANGE_PHYSICS_COMPONENT_PROPERTIES_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    struct PhysicsComponentProperties {
        PhysicsComponentProperties() = default;
        PhysicsComponentProperties(float mass, bool isRotated, const glm::vec3& shapeDimensions)
        : mass(mass), isRotated(isRotated), shapeDimensions(shapeDimensions) {}
        bool operator==(const PhysicsComponentProperties& physicsComponentProperties) {
            return this->mass == physicsComponentProperties.mass
                && this->isRotated == physicsComponentProperties.isRotated
                && this->shapeDimensions == physicsComponentProperties.shapeDimensions;
        }
        bool operator!=(const PhysicsComponentProperties& physicsComponentProperties) {
            return !(*this == physicsComponentProperties);
        }
        float mass;
        bool isRotated;
        glm::vec3 shapeDimensions;
    };

    class ChangePhysicsComponentPropertiesCommand: public IURCommand {
    public:
        ChangePhysicsComponentPropertiesCommand(const Entity& entity, const PhysicsComponentProperties& oldProperties, const PhysicsComponentProperties& newProperties);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        PhysicsComponentProperties _oldProperties;
        PhysicsComponentProperties _newProperties;
    };
}

#endif