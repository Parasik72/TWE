#include "engine.hpp"

namespace TWE {
    int Engine::wndWidth = 0.f;
    int Engine::wndHeight = 0.f;
    std::shared_ptr<DebugCamera> Engine::debugCamera = std::make_shared<DebugCamera>(glm::vec3(0.f, 0.f, 0.f), 0.1f);
    std::shared_ptr<Scene> Engine::curScene;
    Registry<Behavior> Engine::scriptRegistry;
    Registry<MeshSpecification> Engine::meshRegistry;
    Registry<MeshRendererSpecification> Engine::meshRendererRegistry;
    std::unique_ptr<GUI> Engine::gui;

    Engine::Engine(int wndWidth, int wndHeight, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
        //glfw
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(wndWidth, wndHeight, title, monitor, share);
        if(!window){
            std::cout << "Error creating a window.\n";
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, &Engine::keyCallback);
        glfwSetMouseButtonCallback(window, &Engine::mouseButtonCallback);
        glfwSetCursorPos(window, static_cast<GLfloat>(wndWidth / 2), static_cast<GLfloat>(wndHeight / 2));
        glfwSetCursorPosCallback(window, &Engine::mouseCallback);
        glfwSetFramebufferSizeCallback(window, &Engine::framebufferSizeCallback);
        //glad
        gladLoadGL();
        glViewport(0, 0, wndWidth, wndHeight);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        //imgui
        gui = std::make_unique<GUI>(window);
        //initialization vars
        srand(static_cast<unsigned>(time(0)));
        Engine::wndWidth = wndWidth;
        Engine::wndHeight = wndHeight;
        bFillLineMode = true;
        bPreFillLineMode = bFillLineMode;
        debugCamera->setPerspective(90.f, wndWidth, wndHeight);
        curScene = std::make_shared<TWE::Scene>(wndWidth, wndHeight);
        Shape::meshRegistry = &meshRegistry;
        Shape::meshRendererRegistry = &meshRendererRegistry;
        curScene->setDebugCamera(debugCamera.get());
        gui->setScriptRegistry(&scriptRegistry);
        gui->setScene(curScene.get());
        setVSync(true);
    }

    Engine::~Engine(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        Input::keyCallback(window, key, scancode, action, mode);
    }

    void Engine::keyInput(){
        if(!Input::isMouseButtonPressed(Mouse::MOUSE_BUTTON_RIGHT) || !curScene->getIsFocusedOnDebugCamera())
            return;
        if(Input::isKeyPressed(Keyboard::KEY_W))
            debugCamera->keyInput(CamMovement::FORWARD, Time::getDeltaTime());
        if(Input::isKeyPressed(Keyboard::KEY_S))
            debugCamera->keyInput(CamMovement::BACKWARD, Time::getDeltaTime());
        if(Input::isKeyPressed(Keyboard::KEY_D))
            debugCamera->keyInput(CamMovement::RIGHT, Time::getDeltaTime());
        if(Input::isKeyPressed(Keyboard::KEY_A))
            debugCamera->keyInput(CamMovement::LEFT, Time::getDeltaTime());
        if(Input::isKeyPressed(Keyboard::KEY_LEFT_SHIFT))
            debugCamera->keyInput(CamMovement::L_SHIFT, Time::getDeltaTime());
    }

    void Engine::setVSync(GLboolean isOn) {
        vSync = isOn;
        glfwSwapInterval(vSync ? 1 : 0);
    }

    void Engine::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        Input::mouseButtonCallback(window, button, action, mods);
        if(button == GLFW_MOUSE_BUTTON_RIGHT && curScene->getIsFocusedOnDebugCamera()) {
            glfwSetInputMode(window, GLFW_CURSOR, action == GLFW_PRESS ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }

    void Engine::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        static bool debugCameraFlag = false;
        if(!gui->getIsFocusedOnViewport())
            return;
        Input::mouseCallback(window, xpos, ypos);
        if(Input::isMouseButtonPressed(Mouse::MOUSE_BUTTON_RIGHT) && curScene->getIsFocusedOnDebugCamera()) {
            if(debugCameraFlag)
                debugCamera->mouseInput(Input::mouseOffset[0], Input::mouseOffset[1]);
            debugCameraFlag = true;
        } else
            debugCameraFlag = false;
    }

    void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Renderer::setViewport(0, 0, width, height);
        curScene->getFrameBuffer()->resize(width, height);
    }

    void Engine::drawMode() {
        if(bFillLineMode && bPreFillLineMode != bFillLineMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            bPreFillLineMode = bFillLineMode;
        }
        else if(bPreFillLineMode != bFillLineMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            bPreFillLineMode = bFillLineMode;
        }
    }

    void Engine::start(){
        gui->addCheckbox("Fill", bFillLineMode);
        gui->addCheckbox("Debug camera focus", curScene->getIsFocusedOnDebugCamera());
        while(!glfwWindowShouldClose(window)){
            Renderer::cleanScreen({0.25f, 0.25f, 0.25f, 0.f});
            glfwPollEvents();
            keyInput();
            drawMode();
            curScene->updatePhysics();
            curScene->generateShadows(wndWidth, wndHeight);
            curScene->update();
            gui->update();
            glfwSwapBuffers(window);
            Time::calculate();
        }
    }
}