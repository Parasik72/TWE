#include "stream/registry-recorder.hpp"

namespace TWE {
    std::string RegistryRecorder::_registryLoaderPath;

    void RegistryRecorder::recordScript(const std::string& className, const std::string& scriptDirectoryPath) {
        if(!std::filesystem::exists(_registryLoaderPath))
            return;
        std::vector<std::string> fileBody;
        std::ifstream is;
        is.open(_registryLoaderPath);
        for(std::string line; std::getline(is, line);) {
            if(trim(line) == "//" + className)
                return;
            fileBody.push_back(line);
        }
        is.close();

        std::ofstream os;
        os.open(_registryLoaderPath, std::ios::trunc);
        for(auto& line : fileBody) {
            std::string trimLine = trim(line);
            if(trimLine == "//IncludeTail") {
                os << "//" << className << '\n';
                os << "#include \"" << scriptDirectoryPath << "/" << className << ".hpp\"\n";
                os << "//IncludeTail\n";
                continue;
            }
            if(trimLine == "//InstanceTail") {
                os << "\t\t\t" << "registry.add<" << className << ">(\"" << className << "\");\n";
                os << "\t\t\t"  << "//InstanceTail\n";
                continue;
            }
            os << line << '\n';
        }
        os.close();
    }

    void RegistryRecorder::removeScript(const std::string& className) {
        if(!std::filesystem::exists(_registryLoaderPath))
            return;
        std::vector<std::string> fileBody;
        std::ifstream is;
        is.open(_registryLoaderPath);
        for(std::string line; std::getline(is, line);)
            fileBody.push_back(line);
        is.close();

        std::ofstream os;
        os.open(_registryLoaderPath, std::ios::trunc);
        bool skipNext = false;
        std::string includeName = "//" + className;
        std::string registryAddName = "registry.add<" + className + ">(\"" + className + "\");";
        for(auto& line : fileBody) {
            if(skipNext) {
                skipNext = false;
                continue;
            }
            std::string trimLine = trim(line);
            if(trimLine == includeName)
                skipNext = true;
            else if(trimLine != registryAddName)
                os << line << '\n';
        }
        os.close();
    }

    std::string RegistryRecorder::trim(const std::string& str) {
        std::string result = str;
        result.erase(result.find_last_not_of(' ') + 1);
        result.erase(0, result.find_first_not_of(' '));
        result.erase(result.find_last_not_of('\t') + 1);
        result.erase(0, result.find_first_not_of('\t'));
        return result;
    }

    void RegistryRecorder::initPaths(const std::string &registryLoaderPath) {
        _registryLoaderPath = registryLoaderPath;
    }
}