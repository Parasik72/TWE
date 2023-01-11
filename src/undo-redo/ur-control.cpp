#include "undo-redo/ur-control.hpp"

namespace TWE {
    void URControl::undo() {
        if(undoStack.empty())
            return;
        auto command = undoStack.top();
        undoStack.pop();
        command->unExecute();
        redoStack.push(command);
    }

    void URControl::redo() {
        if(redoStack.empty())
            return;
        auto command = redoStack.top();
        redoStack.pop();
        command->execute();
        undoStack.push(command);
    }

    void URControl::execute(IURCommand* command) {
        command->execute();
        undoStack.push(command);
        while(!redoStack.empty()) {
            delete redoStack.top();
            redoStack.pop();
        }
    }

    void URControl::reset() {
        while(!undoStack.empty()) {
            delete undoStack.top();
            undoStack.pop();
        }
        while(!redoStack.empty()) {
            delete redoStack.top();
            redoStack.pop();
        }
    }
}