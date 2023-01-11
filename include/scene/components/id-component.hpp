#ifndef ID_COMPONENT_HPP
#define ID_COMPONENT_HPP

namespace TWE {
    class IDComponent {
    public:
        IDComponent();
        IDComponent(int id);
        IDComponent(const IDComponent& idComponent);
        int id;
    };
}

#endif