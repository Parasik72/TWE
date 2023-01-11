#include "stream/dll-creator.hpp"

namespace TWE {
    std::string DLLCreator::_tempDir;
    
    DLLLoadData DLLCreator::compileScript(const std::string& scriptName, const std::string& scriptDirectoryPath) {
        createScriptDirectory(_tempDir, scriptName);
        createCMakeFile(_tempDir, scriptName);
        createCPPFile(_tempDir, scriptName, scriptDirectoryPath);
        std::string buildDir = _tempDir + '/' + scriptName + "/build"; 
        std::string generateCommand = "cmake -S " + _tempDir + '/' + scriptName + " -B " + buildDir ;
        if(system(generateCommand.c_str()) != 0) {
            std::cout << "Failed to generate script build files\n";
            return {};
        }
        std::string buildCommand = "cmake --build " + buildDir;
        if(system(buildCommand.c_str()) != 0) {
            std::cout << "Failed to build script\n";
            return {};
        }
        std::string dllPath = buildDir + "/Debug/" + scriptName + ".dll";
        std::string pdbPath = buildDir + "/Debug/" + scriptName + ".pdb";
        std::string factoryFuncName = "create" + scriptName;
        std::cout << dllPath << std::endl;
        std::cout << factoryFuncName << std::endl;
        File::remove(pdbPath.c_str());
        return { dllPath, factoryFuncName, scriptName, scriptDirectoryPath };
    }

    PVoid DLLCreator::loadDLLFunc(DLLLoadData& loadData) {
        #if defined(TWE_PLATFORM_WINDOWS)
        HINSTANCE hDll = LoadLibrary(loadData.dllPath.c_str());
        if (hDll == NULL) {
            std::cout << "Failed to load library.\n";
            return nullptr;
        }
        loadData.hDlls.push_back(hDll);
        return (PVoid)GetProcAddress(hDll, loadData.factoryFuncName.c_str());
        #else
        return nullptr;
        #endif
    }

    void DLLCreator::freeDLLFunc(DLLLoadData& loadData) {
        #if defined(TWE_PLATFORM_WINDOWS)
        for(auto& hdll : loadData.hDlls)
            FreeLibrary(hdll);
        loadData.hDlls.clear();
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

    void DLLCreator::createCMakeFile(const std::string& tempDir, const std::string& scriptName) {
        std::string cmakeFilePath = tempDir + '/' + scriptName + "/CMakeLists.txt";
        if(std::filesystem::exists(cmakeFilePath))
            return;;
        std::ofstream os;
        os.open(cmakeFilePath);
        os << "cmake_minimum_required(VERSION 3.20)\n";
        os << "project(" + scriptName << ")\n";

        os << "file(GLOB_RECURSE CPPFILES\n";
        os << "\"" + fixPath(tempDir) + "/" + scriptName + "/" + scriptName + ".cpp\")\n";

        os << "file(GLOB_RECURSE LIBFILES\n";
        os << "\"" + fixPath("../../lib") + "/*.lib\"\n";
        os << "\"" + fixPath("../../external") + "/*.lib\")\n";

        os << "add_library(" + scriptName << " MODULE ${CPPFILES})\n";
        os << "target_compile_features(" + scriptName + " PRIVATE cxx_std_17)\n";
        os << "target_include_directories(" + scriptName + " PRIVATE\n";
        os << "\"" + fixPath("../../include") + "\"\n";
        os << "\"" + fixPath("../../external") + "\"\n";
        os << "\"" + fixPath("../../external/GLFW") + "\"\n";
        os << "\"" + fixPath("../../external/glad") + "\"\n";
        os << "\"" + fixPath("../../external/stb") + "\"\n";
        os << "\"" + fixPath("../../external/glm") + "\"\n";
        os << "\"" + fixPath("../../external/imgui") + "\"\n";
        os << "\"" + fixPath("../../external/imgui-filedialog") + "\"\n";
        os << "\"" + fixPath("../../external/ImGuizmo") + "\"\n";
        os << "\"" + fixPath("../../external/assimp") + "\"\n";
        os << "\"" + fixPath("../../external/bullet3") + "\"\n";
        os << "\"" + fixPath("../../external/entt") + "\"\n";
        os << "\"" + fixPath("../../external/json") + "\")\n";

        os << "target_link_libraries(" + scriptName + " PRIVATE ${LIBFILES})\n";

        os.close();
    }

    std::string DLLCreator::fixPath(const std::string& path) {
        std::string res = std::filesystem::absolute(path).string();
        std::replace(res.begin(), res.end(), '\\', '/');
        return res;
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

    void DLLCreator::initPaths(const std::string& tempDir) {
        _tempDir = tempDir;
    }

    void DLLCreator::removeScript(const std::string& scriptName) {
        if(!std::filesystem::exists(_tempDir))
            return;
        std::string scriptTempFolderPath = _tempDir + '/' + scriptName;
        if(!std::filesystem::exists(scriptTempFolderPath))
            return;
        std::filesystem::remove_all(scriptTempFolderPath);
    }
}