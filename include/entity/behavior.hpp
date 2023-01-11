#ifndef BEHAVIOR_HPP
#define BEHAVIOR_HPP

#include <string>
#include <glm.hpp>
#include <btBulletDynamicsCommon.h>

#include "entity.hpp"
#include "input/input.hpp"

namespace TWE {
    struct InputSpecification {
    public:
        InputSpecification() = default;
        InputSpecification(bool* keyboardPressedKeys, bool* mousePressedButtons, float* mouseOffset, int* keyboardPressedActions, int* mousePressedActions)
            : keyboardPressedKeys(keyboardPressedKeys), mousePressedButtons(mousePressedButtons), 
              mouseOffset(mouseOffset), keyboardPressedActions(keyboardPressedActions), mousePressedActions(mousePressedActions) {}
        bool isKeyPressed(Keyboard key) { 
            if(keyboardPressedKeys[key] && keyboardPressedActions[key] == Action::PRESS)
                keyboardPressedActions[key] = Action::REPEAT;
            return keyboardPressedKeys[key];
        }
        bool isKeyPressedOnce(Keyboard key) {
            if(keyboardPressedKeys[key] && keyboardPressedActions[key] == Action::PRESS) {
                keyboardPressedActions[key] = Action::REPEAT;
                return true;
            }
            return false;
        }
        bool isMouseButtonPressed(Mouse button) { 
            if(mousePressedButtons[button] && mousePressedActions[button] == Action::PRESS)
                mousePressedActions[button] = Action::REPEAT;
            return mousePressedButtons[button];
        }
        bool isMouseButtonPressedOnce(Mouse button) {
            if(mousePressedButtons[button] && mousePressedActions[button] == Action::PRESS) {
                mousePressedActions[button] = Action::REPEAT;
                return true;
            }
            return false;
        }
        Action keyAction(Keyboard key) { return static_cast<Action>(keyboardPressedActions[key]); }
        Action mouseButtonAction(Mouse button) { return static_cast<Action>(mousePressedActions[button]); }
        glm::vec2 getMouseOffset() { return { mouseOffset[0], mouseOffset[1] }; }
    private:
        bool* keyboardPressedKeys;
        int* keyboardPressedActions;
        bool* mousePressedButtons;
        int* mousePressedActions;
        float* mouseOffset;
    };

    class Behavior {
    public:
        virtual void start() {}
        virtual void update(float deltaTime) {}
        virtual void collisionDetection(Entity collidedEntity, const btCollisionObject* collisionObj) {}
    protected:
        template<typename T>
        bool hasComponent();
        template<typename T>
        T* getComponent();
        template<typename T, typename ...Args>
        T* addComponent(Args&&... args);
        template<typename T>
        void removeComponent();
        template<typename T = void>
        void destroy();
        InputSpecification input;
        Entity gameObject;
    private:
        void setInput(bool* keyboardPressedKeys, bool* mousePressedButtons, float* mouseOffset, int* keyboardPressedActions, int* mousePressedActions) {
            input = { keyboardPressedKeys, mousePressedButtons, mouseOffset, keyboardPressedActions, mousePressedActions };
        }
        friend class ScriptComponent;
        friend class Scene;
    };

    template<typename T>
    bool Behavior::hasComponent() {
        return gameObject.hasComponent<T>();
    }

    template<typename T>
    T* Behavior::getComponent() {
        if(!hasComponent<T>()) {
            std::string componentName = typeid(T).name();
            componentName = componentName.substr(6);
            throw std::runtime_error("Error: A " + componentName + " was not found.");
        }
        return &gameObject.getComponent<T>();
    }

    template<typename T, typename ...Args>
    T* Behavior::addComponent(Args&&... args) {
        if(hasComponent<T>()) {
            std::string componentName = typeid(T).name();
            componentName = componentName.substr(6);
            throw std::runtime_error("Error: A " + componentName + " is already exists.");
        }
        return &gameObject.addComponent<T, Args>(args);
    }

    template<typename T>
    void Behavior::removeComponent() {
        gameObject.removeComponent<T>();
    }

    template<typename T>
    void Behavior::destroy() {
        gameObject.destroy();
    }
}

#endif