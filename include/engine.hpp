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
#include "model-loader/model-loader.hpp"
#include "entity/entity.hpp"
#include "input/input.hpp"
#include "registry/registry.hpp"
#include "stream/project-creator.hpp"
#include "stream/build-creator.hpp"

#ifndef TWE_BUILD
#include "gui/gui.hpp"
#else
#include "scene/scene-serializer.hpp"
#endif

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
        std::string windowTitle;
        static std::unique_ptr<ProjectData> projectData;
        static std::shared_ptr<Scene> curScene;
        static std::shared_ptr<DebugCamera> debugCamera;
        static Registry<DLLLoadData> scriptDLLRegistry;
        static Registry<MeshSpecification> meshRegistry;
        static Registry<MeshRendererSpecification> meshRendererRegistry;
        #ifndef TWE_BUILD
        static std::unique_ptr<GUI> gui;
        #endif
    private:
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    };
}

#endif