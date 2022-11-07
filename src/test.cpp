#include "test.hpp"

namespace TWE {
    Test::Test(int wndWidth, int wndHeight, const char* title, GLFWmonitor *monitor, GLFWwindow *share)
    :Engine(wndWidth, wndHeight, title, monitor, share){
        auto& objEntity = Shape::createCubeEntity(curScene.get(), {"../../textures/box.png"});
        auto& transformComponentObj = objEntity.getComponent<TransformComponent>();
        auto& physicsComponentObj = objEntity.getComponent<PhysicsComponent>();
        transformComponentObj.move({0.f, 0.01f, 3.f});
        physicsComponentObj.setWorldTransform({0.f, 0.01f, 3.f});

        auto& obj2Entity = Shape::createCubeEntity(curScene.get(), {"../../textures/ok.png"});
        auto& transformComponentObj2 = obj2Entity.getComponent<TransformComponent>();
        auto& physicsComponentObj2 = obj2Entity.getComponent<PhysicsComponent>();
        physicsComponentObj2.setWorldTransform({0.9f, 10.f, 0.9f});
        physicsComponentObj2.setMass(curScene->getDynamicWorld(), 1.f);

        auto& obj3Entity = Shape::createCubeEntity(curScene.get());
        auto& transformComponentObj3 = obj3Entity.getComponent<TransformComponent>();
        auto& physicsComponentObj3 = obj3Entity.getComponent<PhysicsComponent>();
        physicsComponentObj3.setWorldTransform({-1.5f, 1.f, 0.f});
        transformComponentObj3.move({-1.5f, 1.f, 0.f});
        transformComponentObj3.scale({0.5f, 0.5f, 0.5f});
        obj3Entity.addComponent<ScriptComponent>().bind<RotateRGBCube>();

        auto& obj4Entity = Shape::createCubeEntity(curScene.get(), {"../../textures/ok.png"});
        auto& transformComponentObj4 = obj4Entity.getComponent<TransformComponent>();
        auto& physicsComponentObj4 = obj4Entity.getComponent<PhysicsComponent>();
        physicsComponentObj4.setWorldTransform({1.7f, 11.f, 0.9f});
        physicsComponentObj4.setMass(curScene->getDynamicWorld(), 1.f);

        Shape::createPlateEntity(curScene.get());

        auto& spotLightEntity = Shape::createSpotLightEntity(curScene.get());
        auto& transformComponentSL = spotLightEntity.getComponent<TransformComponent>();
        auto& meshRendererComponentSL = spotLightEntity.getComponent<MeshRendererComponent>();
        transformComponentSL.move({6.5f, 2.f, -4.5f});
        transformComponentSL.rotate(45.f, {0.f, -1.f, 0.f});
        transformComponentSL.rotate(15.f, {1.f, 0.f, 0.f});
        meshRendererComponentSL.material->objColor = {0.8f, 0.25f, 0.25f};

        auto& spotLight2Entity = Shape::createSpotLightEntity(curScene.get());
        auto& transformComponent2SL = spotLight2Entity.getComponent<TransformComponent>();
        transformComponent2SL.move({4.f, 2.f, -4.5f});
        transformComponent2SL.rotate(25.f, {0.f, -1.f, 0.f});
        transformComponent2SL.rotate(15.f, {1.f, 0.f, 0.f});

        auto& pointLightEntity = Shape::createPointLightEntity(curScene.get());
        auto& transformComponentPL = pointLightEntity.getComponent<TransformComponent>();
        auto& meshRendererComponentPL = pointLightEntity.getComponent<MeshRendererComponent>();
        transformComponentPL.move({-2.5f, 0.25f, -1.f});
        meshRendererComponentPL.material->objColor = {0.f, 1.f, 1.f};

        auto& dirLightEntity = Shape::createDirLightEntity(curScene.get());
        auto& transformComponentDL = dirLightEntity.getComponent<TransformComponent>();
        transformComponentDL.move({5.f, 5.f, 3.f});
        transformComponentDL.rotate(90.f, {1.f, 0.f, 0.f});
        dirLightEntity.addComponent<ScriptComponent>().bind<DirBehavior>();

        ModelLoader mLoader;
        ModelLoaderData* model = mLoader.loadModel("../../models/tophat/TopHat.obj");
        if(model){
            auto& modelEntities = Shape::createModelEntity(curScene.get(), model);
            for(auto& modelEntitie : modelEntities) {
                auto& transformComponentM = modelEntitie.getComponent<TransformComponent>();
                transformComponentM.scale({0.2f, 0.2f, 0.2f});
            }
        }

        auto& cubemapEntity = Shape::createCubemapEntity(
            curScene.get(),
            {
                "../../textures/cubemap/posx.jpg",
                "../../textures/cubemap/negx.jpg",
                "../../textures/cubemap/posy.jpg",
                "../../textures/cubemap/negy.jpg",
                "../../textures/cubemap/posz.jpg",
                "../../textures/cubemap/negz.jpg",
            }
        );

        auto& cameraEntity = Shape::createCameraEntity(curScene.get());
        auto& transformComponentCam = cameraEntity.getComponent<TransformComponent>();
        auto& cameraComponentCam = cameraEntity.getComponent<CameraComponent>();
        cameraEntity.addComponent<ScriptComponent>().bind<CameraController>();
        transformComponentCam.move({0.f, 1.f, 8.f});
        cameraComponentCam.getSource()->setPerspective(90.f, wndWidth, wndHeight);
        // cameraComponentCam.getSource()->setOrthographic(-5.f, 5.f, -5.f, 5.f);

        debugCamera->setPosition({0.f, 1.f, 6.f});
    }
}