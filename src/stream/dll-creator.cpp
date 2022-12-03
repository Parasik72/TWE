#include "stream/dll-creator.hpp"

namespace TWE {
    DLLLoadData DLLCreator::compileScript(const std::string& tempDir, const std::string& scriptName, const std::string& scriptDirectoryPath,
    const std::string& openCLSDKPath) {
        createScriptDirectory(tempDir, scriptName);
        createCMakeFile(tempDir, scriptName, openCLSDKPath);
        createCPPFile(tempDir, scriptName, scriptDirectoryPath);
        std::string buildDir = tempDir + '/' + scriptName + "/build"; 
        std::string generateCommand = "cmake -S " + tempDir + '/' + scriptName + " -B " + buildDir ;
        if(system(generateCommand.c_str()) != 0) {
            std::cout << "Failed to generate build files\n";
            return {};
        }
        std::string buildCommand = "cmake --build " + buildDir;
        if(system(buildCommand.c_str()) != 0) {
            std::cout << "Failed to build script\n";
            return {};
        }
        std::string dllPath = buildDir + "/Debug/" + scriptName + ".dll";
        std::string factoryFuncName = "create" + scriptName;
        std::cout << dllPath << std::endl;
        std::cout << factoryFuncName << std::endl;
        return { dllPath, factoryFuncName, scriptName };
    }

    PVoid DLLCreator::loadDLLFunc(const std::string& dllPath, const std::string& factoryFuncName) {
        #if defined(TWE_PLATFORM_WINDOWS)
        HINSTANCE hDll = LoadLibrary(dllPath.c_str());
        if (hDll == NULL) {
            std::cout << "Failed to load library.\n";
            return nullptr;
        }
        return (PVoid)GetProcAddress(hDll, factoryFuncName.c_str());
        #else
        return nullptr;
        #endif
    }

    PVoid DLLCreator::loadDLLFunc(const DLLLoadData& loadData) {
        #if defined(TWE_PLATFORM_WINDOWS)
        HINSTANCE hDll = LoadLibrary(loadData.dllPath.c_str());
        if (hDll == NULL) {
            std::cout << "Failed to load library.\n";
            return nullptr;
        }
        return (PVoid)GetProcAddress(hDll, loadData.factoryFuncName.c_str());
        #else
        return nullptr;
        #endif
    }

    void DLLCreator::createScriptDirectory(const std::string& tempDir, const std::string& scriptName) {
        if(!std::filesystem::exists(tempDir))
            std::filesystem::create_directory(tempDir);
        std::string scriptDir = tempDir + '/' + scriptName;
        if(!std::filesystem::exists(scriptDir))
            std::filesystem::create_directory(scriptDir);
        std::string buildDir = scriptDir + "/build";
        if(!std::filesystem::exists(buildDir))
            std::filesystem::create_directory(buildDir);
    }

    void DLLCreator::createCMakeFile(const std::string& tempDir, const std::string& scriptName, const std::string& openCLSDKPath) {
        std::string cmakeFilePath = tempDir + '/' + scriptName + "/CMakeLists.txt";
        if(std::filesystem::exists(cmakeFilePath))
            return;
        std::ofstream os;
        os.open(cmakeFilePath);
        os << "cmake_minimum_required(VERSION 3.20)\n";
        os << "project(" + scriptName << ")\n";

        os << "file(GLOB_RECURSE CPPFILES\n";
        os << "\"" + tempDir + "/" + scriptName + "/" + scriptName + ".cpp\")\n";

        os << "file(GLOB_RECURSE LIBFILES\n";
        os << "\"../../lib/*.lib\"\n";
        os << "\"../../external/*.lib\"\n";
        os << "\"" << openCLSDKPath + "/lib/*.lib\")\n";

        os << "add_library(" + scriptName << " MODULE ${CPPFILES})\n";
        os << "target_compile_features(" + scriptName + " PRIVATE cxx_std_17)\n";
        os << "target_include_directories(" + scriptName + " PRIVATE\n";
        os << "\"../../include\"\n";
        os << "\"../../external\"\n";
        os << "\"../../external/GLFW\"\n";
        os << "\"../../external/glad\"\n";
        os << "\"../../external/stb\"\n";
        os << "\"../../external/glm\"\n";
        os << "\"../../external/imgui\"\n";
        os << "\"../../external/imgui-filedialog\"\n";
        os << "\"../../external/ImGuizmo\"\n";
        os << "\"../../external/assimp\"\n";
        os << "\"../../external/bullet3\"\n";
        os << "\"../../external/entt\"\n";
        os << "\"../../external/json\"\n";
        os << "\"" << openCLSDKPath + "/include\")\n";

        os << "target_link_libraries(" + scriptName + " PRIVATE ${LIBFILES})\n";

        os.close();
    }

    void DLLCreator::createCPPFile(const std::string& tempDir, const std::string& scriptName, const std::string& scriptDirectoryPath) {
        std::string cppFilePath = tempDir + "/" + scriptName + "/" + scriptName + ".cpp";
        if(std::filesystem::exists(cppFilePath))
            return;
        std::ofstream os;
        os.open(cppFilePath);

        os << "#include " << "\"" << scriptDirectoryPath + "/" + scriptName << ".hpp\"\n";
        os << "Behavior* create" << scriptName << "(){\n"; 
        os << "\treturn new " + scriptName + "();\n"; 
        os << "}\n"; 

        os.close();
    }
}