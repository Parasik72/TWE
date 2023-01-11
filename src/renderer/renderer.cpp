#include "renderer/renderer.hpp"

namespace TWE {
    void Renderer::execute(MeshComponent* meshComponent, MeshRendererComponent* meshRendererComponent, TransformComponent* transformComponent, 
    int lightsCount) {
        meshRendererComponent->shader->use();
        meshRendererComponent->shader->setUniform(TRANS_MAT_OPTIONS[TransformMatrixOptions::MODEL], transformComponent->getModel());
        meshRendererComponent->shader->setUniform("hasTexture", !meshComponent->texture->getAttachments().textureSpecifications.empty());
        meshRendererComponent->shader->setUniform("lightCount", lightsCount);
        meshRendererComponent->updateMaterialUniform();
        meshComponent->vao->bind();
        meshComponent->texture->bind();
        glDrawElements(GL_TRIANGLES, meshComponent->ebo->getSize() / sizeof(int), GL_UNSIGNED_INT, (void*)0);
        meshComponent->vao->unbind();
    }

    void Renderer::cleanScreen(const glm::vec4& color) {
        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        static const float red[] = { 1, 0, 0, 1 };
        glClearBufferfv(GL_COLOR, 1, red);
    }

    void Renderer::setViewport(int startX, int startY, int endX, int endY) {
        glViewport(startX, startY, endX, endY);
    }

    void Renderer::setLight(MeshRendererComponent& meshRendererComponent, const LightComponent& light, TransformComponent& transform, const std::string& lightIndex) {
        meshRendererComponent.shader->setUniform((lightIndex + ".pos").c_str(), transform.transform.position);
        meshRendererComponent.shader->setUniform((lightIndex + ".direction").c_str(), transform.getForward());
        meshRendererComponent.shader->setUniform((lightIndex + ".color").c_str(), light.color);
        meshRendererComponent.shader->setUniform((lightIndex + ".cutOff").c_str(), glm::cos(glm::radians(light.innerRadius)));
        meshRendererComponent.shader->setUniform((lightIndex + ".outerCutOff").c_str(), glm::cos(glm::radians(light.outerRadius)));
        meshRendererComponent.shader->setUniform((lightIndex + ".castShadows").c_str(), light.castShadows);
        meshRendererComponent.shader->setUniform((lightIndex + ".fading.constant").c_str(), light.constant);
        meshRendererComponent.shader->setUniform((lightIndex + ".fading.linear").c_str(), light.linear);
        meshRendererComponent.shader->setUniform((lightIndex + ".fading.quadratic").c_str(), light.quadratic);
        for(auto& type : lightTypes)
            meshRendererComponent.shader->setUniform((lightIndex + ".type." + type).c_str(), lightTypes[light.type] == type);
    }

    void Renderer::setViewPosition(MeshRendererComponent& meshRendererComponent, const glm::vec3& pos) {
        meshRendererComponent.shader->setUniform("viewPos", pos);
    }

    void Renderer::setShadows(MeshRendererComponent& meshRendererComponent, const glm::mat4& lightSpaceMat, const std::string& lightIndex) {
        meshRendererComponent.shader->setUniform((lightIndex + ".lightSpaceMat").c_str(), lightSpaceMat);
        meshRendererComponent.shader->setUniform((lightIndex + ".shadowMap").c_str(), 31);
    }

    void Renderer::generateDepthMap(LightComponent& lightComponent, const TransformComponent& transformComponent, const glm::mat4& lightProjection, 
    const glm::mat4& lightView, Scene* scene) {
        scene->updateView(lightView, lightProjection, transformComponent.transform.position);
        auto& depthMapSize = lightComponent.getDepthMapSize();
        auto fbo = lightComponent.getFBO();
        glActiveTexture(GL_TEXTURE31);
        glBindTexture(GL_TEXTURE_2D, lightComponent.getDepthTextureId());
        Renderer::setViewport(0, 0, depthMapSize.first, depthMapSize.second);
        glCullFace(GL_FRONT);
        fbo->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        scene->draw();
        fbo->unbind();
        glCullFace(GL_BACK);
    }
}