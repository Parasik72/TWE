#ifndef UR_CONTROL_HPP
#define UR_CONTROL_HPP

#include <stack>

#include "undo-redo/iur-command.hpp"

namespace TWE {
    class URControl {
    public:
        void undo();
        void redo();
        void execute(IURCommand* command);
        void reset();
    private:
        std::stack<IURCommand*> undoStack;
        std::stack<IURCommand*> redoStack;
    };
}

#endif