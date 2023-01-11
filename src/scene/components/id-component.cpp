#include "scene/components/id-component.hpp"

namespace TWE {
    IDComponent::IDComponent(): id(-1) {}

    IDComponent::IDComponent(int id): id(id) {}

    IDComponent::IDComponent(const IDComponent& idComponent) {
        this->id = idComponent.id;
    }
}