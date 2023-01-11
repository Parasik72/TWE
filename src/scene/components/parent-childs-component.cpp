#include "scene/components/parent-childs-component.hpp"

namespace TWE {
    ParentChildsComponent::ParentChildsComponent(): parent(entt::null) {}

    ParentChildsComponent::ParentChildsComponent(const ParentChildsComponent& parentChildsComponent) {
        this->parent = parentChildsComponent.parent;
        this->childs = parentChildsComponent.childs;
    }
}