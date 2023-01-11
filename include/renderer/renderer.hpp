#ifndef REDERER_HPP
#define REDERER_HPP

#include <vector>
#include <string>
#include <entt/entt.hpp>

#include "scene/components/components.hpp"
#include "scene/scene.hpp"
#include "renderer/texture.hpp"

namespace TWE {
    class Renderer {
    public:
        static void execute(MeshComponent* meshComponent, MeshRendererComponent* meshRendererComponent, TransformComponent* transformComponent, 
            int lightsCount);
        static void cleanScreen(const glm::vec4& color);
        static void setViewport(int startX, int startY, int endX, int endY);
        static void setLight(MeshRendererComponent& meshRendererComponent, const LightComponent& light, TransformComponent& transform, const std::string& lightIndex);
        static void setViewPosition(MeshRendererComponent& meshRendererComponent, const glm::vec3& pos);
        static void setShadows(MeshRendererComponent& meshRendererComponent, const glm::mat4& lightSpaceMat, const std::string& lightIndex);
        static void generateDepthMap(LightComponent& lightComponent, const TransformComponent& transformComponent, const glm::mat4& lightProjection, 
                                     const glm::mat4& lightView, Scene* scene);
    };
}

#endif