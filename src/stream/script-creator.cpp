#include "stream/script-creator.hpp"

namespace TWE {
    bool ScriptCreator::create(const std::string& scriptName, const std::string& scriptDirectoryPath) {
        if(!validateScriptName(scriptName))
            return false;
        if(!std::filesystem::exists(scriptDirectoryPath))
            return false;
        std::string scriptFilePath =  scriptDirectoryPath + '/' + scriptName + ".hpp";
        if(std::filesystem::exists(scriptFilePath))
            return false;
        std::ofstream os;
        os.open(scriptFilePath);

        os << "#pragma once\n\n";
        os << "#include \"entity/behavior.hpp\"\n";
        os << "#include \"scene/scene.hpp\"\n";
        os << "#include \"scene/components/components.hpp\"\n\n";
        os << "using namespace TWE;\n\n";
        os << "class " << scriptName << " : public Behavior {\n";
        os << "public:\n";
        os << "\tvoid start() override {\n\t\t\n\t}\n\n";
        os << "\tvoid update(float deltaTime) override {\n\t\t\n\t}\n};\n\n";
        os << "extern \"C\" __declspec(dllexport) Behavior* create" << scriptName << "();";
       
        os.close();
        return true;
    }

    bool ScriptCreator::validateScriptName(const std::string& scriptName) {
        return std::regex_match(scriptName, std::regex("^[a-zA-Z][a-zA-Z0-9]+"));
    }
}