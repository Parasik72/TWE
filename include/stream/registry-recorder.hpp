#ifndef REGISTRY_RECORDER_HPP
#define REGISTRY_RECORDER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace TWE {
    class RegistryRecorder {
    public:
        static void recordScript(const std::string& className, const std::string& scriptDirectoryPath);
        static void initPaths(const std::string &registryLoaderPath);
    private:
        static std::string _registryLoaderPath;
        [[nodiscard]] static std::string trim(const std::string& str);
    };
}

#endif