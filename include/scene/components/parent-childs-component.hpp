#ifndef PARENT_CHILDS_COMPONENT_HPP
#define PARENT_CHILDS_COMPONENT_HPP

#include <vector>

#include "entt/entt.hpp"

namespace TWE {
    class ParentChildsComponent {
    public:
        ParentChildsComponent();
        ParentChildsComponent(const ParentChildsComponent& parentChildsComponent);
        entt::entity parent;
        std::vector<entt::entity> childs;
    };
}

#endif