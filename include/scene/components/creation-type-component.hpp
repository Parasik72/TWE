#ifndef CREATION_TYPE_COMPONENT
#define CREATION_TYPE_COMPONENT

#include <string>

namespace TWE {
    enum class EntityCreationType {
        None = -1,
        Cube,
        Plate,
        Cubemap,
        SpotLight,
        PointLight,
        DirLight,
        Camera,
        Model
    };

    class CreationTypeComponent {
    public:
        CreationTypeComponent();
        CreationTypeComponent(EntityCreationType type);
        CreationTypeComponent(const CreationTypeComponent& creationTypeComponent);
        void setType(EntityCreationType type);
        [[nodiscard]] EntityCreationType getType() const noexcept;
    private:
        EntityCreationType _type;
    };
}

#endif