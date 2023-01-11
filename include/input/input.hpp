#ifndef INPUT_HPP
#define INPUT_HPP

#include <vector>

#include "mouse.hpp"
#include "keyboard.hpp"
#include "action.hpp"

namespace TWE {
    class Input {
    public:
        static void init(bool* keyboardPressedKeys, bool* mousePressedButtons, float* mouseOffset);
        [[nodiscard]] static bool isKeyPressed(Keyboard key);
        [[nodiscard]] static bool isKeyPressedOnce(Keyboard key);
        [[nodiscard]] static bool isMouseButtonPressed(Mouse button);
        [[nodiscard]] static bool isMouseButtonPressedOnce(Mouse button);
        [[nodiscard]] static Action keyAction(Keyboard key);
        [[nodiscard]] static Action mouseButtonAction(Mouse button);
    private:
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static bool* keyboardPressedKeys;
        static int* keyboardPressedActions;
        static bool* mousePressedButtons;
        static int* mousePressedActions;
        static float* mouseOffset;
        friend class Engine;
        friend class Scene;
        friend class Behavior;
        friend class ScriptComponent;
    };
}

#endif