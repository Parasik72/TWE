#ifndef IUR_COMMAND_HPP
#define IUR_COMMAND_HPP

#include <string>

namespace TWE {
    class IURCommand {
    public:
        virtual void execute() {}
        virtual void unExecute() {}
        std::string name;
    };
}

#endif