#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "math.h"
#include "scene/shape.hpp"
#include "scene/scene.hpp"
#include "scene/time.hpp"
#include "gui/gui.hpp"
#include "model-loader/model-loader.hpp"
#include "entity/entity.hpp"
#include "input/input.hpp"
#include "registry/registry.hpp"

namespace TWE {
    class Engine {
    public:
        Engine(int wndWidth, int wndHeight, const char* title, GLFWmonitor *monitor, GLFWwindow *share);
        ~Engine();
        void start();
    protected:
        void keyInput();
        void setVSync(GLboolean isOn);
        GLboolean vSync;
        GLFWwindow* window;
        static int wndWidth;
        static int wndHeight;
        static std::unique_ptr<GUI> gui;
        static std::shared_ptr<Scene> curScene;
        static std::shared_ptr<DebugCamera> debugCamera;
        static Registry<Behavior> scriptRegistry;
        static Registry<DLLLoadData> scriptDLLRegistry;
        static Registry<MeshSpecification> meshRegistry;
        static Registry<MeshRendererSpecification> meshRendererRegistry;
    private:
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    };
}

#endif