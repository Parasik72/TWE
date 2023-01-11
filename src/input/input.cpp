#include "input/input.hpp"

namespace TWE {
    bool* Input::keyboardPressedKeys = new bool[512]{0};
    bool* Input::mousePressedButtons = new bool[24]{0};
    float* Input::mouseOffset = new float[2]{0};
    int* Input::keyboardPressedActions = new int[512]{0};
    int* Input::mousePressedActions = new int[512]{0};

    void Input::init(bool* keyboardPressedKeysA, bool* mousePressedButtonsA, float* mouseOffsetA) {
        keyboardPressedKeys = keyboardPressedKeysA;
        mousePressedButtons = mousePressedButtonsA;
        mouseOffset = mouseOffsetA;
    }
    
    bool Input::isKeyPressed(Keyboard key) {
        if(keyboardPressedKeys[key] && keyboardPressedActions[key] == Action::PRESS)
            keyboardPressedActions[key] = Action::REPEAT;
        return keyboardPressedKeys[key];
    }

    bool Input::isKeyPressedOnce(Keyboard key) {
        if(keyboardPressedKeys[key] && keyboardPressedActions[key] == Action::PRESS) {
            keyboardPressedActions[key] = Action::REPEAT;
            return true;
        }
        return false;
    }

    bool Input::isMouseButtonPressed(Mouse button) {
        if(mousePressedButtons[button] && mousePressedActions[button] == Action::PRESS)
            mousePressedActions[button] = Action::REPEAT;
        return mousePressedButtons[button];
    }

    bool Input::isMouseButtonPressedOnce(Mouse button) {
        if(mousePressedButtons[button] && mousePressedActions[button] == Action::PRESS) {
            mousePressedActions[button] = Action::REPEAT;
            return true;
        }
        return false;
    }

    Action Input::keyAction(Keyboard key) {
        return static_cast<Action>(keyboardPressedActions[key]);
    }

    Action Input::mouseButtonAction(Mouse button) {
        return static_cast<Action>(mousePressedActions[button]);
    }

    void Input::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        static float preViewX = xpos;
        static float preViewY = ypos;
        mouseOffset[0] = xpos - preViewX;
        mouseOffset[1] = preViewY - ypos;
        preViewX = xpos;
        preViewY = ypos;
    }

    void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if(action == Action::PRESS)
            mousePressedButtons[button] = true;
        if(action == Action::RELEASE)
            mousePressedButtons[button] = false;
        mousePressedActions[button] = action;
    }

    void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if(action == Action::PRESS)
            keyboardPressedKeys[key] = true;
        if(action == Action::RELEASE)
            keyboardPressedKeys[key] = false;
        keyboardPressedActions[key] = action;
    }
}