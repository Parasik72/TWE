#include "stream/build-creator.hpp"

namespace TWE {
    bool BuildCreator::create(ProjectData* projectData, const std::filesystem::path& startScenePath) {
        if(!projectData)
            return false;
        if(projectData->rootPath.empty())
            return false;
        if(!std::filesystem::exists(projectData->rootPath.string() + '/' + startScenePath.string()))
            return false;
        std::string buildDirPath = projectData->rootPath.string() + "/build";
        if(!std::filesystem::exists(buildDirPath))
            std::filesystem::create_directory(buildDirPath);
        std::string debugPath = buildDirPath + "/Debug";
        if(!std::filesystem::exists(debugPath))
            std::filesystem::create_directory(debugPath);
        std::string buildFilePath = debugPath + '/' + projectData->projectName + ".build";
        createBuildFile(projectData, buildFilePath, startScenePath);
        createCMakeFile(projectData, buildDirPath);
        copyRootPathFiles(projectData, buildDirPath);
        std::string generateCommand = "cmake -S " + buildDirPath + " -B " + buildDirPath;
        if(system(generateCommand.c_str()) != 0) {
            std::cout << "Failed to generate project build files\n";
            return false;
        }
        std::string buildCommand = "cmake --build " + buildDirPath;
        if(system(buildCommand.c_str()) != 0) {
            std::cout << "Failed to build project\n";
            return false;
        }
        return true;
    }

    void BuildCreator::copyRootPathFiles(ProjectData* projectData, const std::filesystem::path& buildDirPath) {
        auto dirIt = std::filesystem::directory_iterator(projectData->rootPath);
        std::string debugPath = buildDirPath.string() + "/Debug";
        const auto copyOptions = std::filesystem::copy_options::update_existing
                               | std::filesystem::copy_options::recursive;
        for(auto& entry : dirIt) {
            auto& path = entry.path();
            if(path.filename().string() != "build") {
                if(entry.is_directory()) {
                    std::filesystem::path dirPath = debugPath + '/' + path.filename().string();
                    std::filesystem::create_directory(dirPath);
                    std::filesystem::copy(path, dirPath, copyOptions);
                } else
                    std::filesystem::copy(path, debugPath, copyOptions);
            }
        }
        std::string projectShadersPath = debugPath + "/shaders";
        std::filesystem::create_directory(projectShadersPath);
        std::filesystem::copy("../../shaders", projectShadersPath, copyOptions);
    }

    void BuildCreator::createCMakeFile(ProjectData* projectData, const std::filesystem::path& buildDirPath) {
        std::string cmakeFilePath = buildDirPath.string() + "/CMakeLists.txt";
        if(std::filesystem::exists(cmakeFilePath))
            std::filesystem::remove(cmakeFilePath);
        std::ofstream os;
        os.open(cmakeFilePath);
        os << "cmake_minimum_required(VERSION 3.20)\n";
        os << "project(" + projectData->projectName << ")\n";

        os << "file(GLOB_RECURSE CPPFILES\n";
        os << "\"" + fixPath("../../src") + "/*.cpp\")\n";

        os << "file(GLOB_RECURSE LIBFILES\n";
        os << "\"" +  fixPath("../../external") + "/*.lib\")\n";
        os << "file(MAKE_DIRECTORY \"${CMAKE_BINARY_DIR}/Debug\" \"${CMAKE_BINARY_DIR}/Release\")\n";

        os << "add_executable(" + projectData->projectName << " ${CPPFILES})\n";
        os << "target_compile_features(" + projectData->projectName + " PRIVATE cxx_std_17)\n";
        os << "target_include_directories(" + projectData->projectName + " PRIVATE\n";
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

        os << "target_link_libraries(" + projectData->projectName + " PRIVATE ${LIBFILES})\n";

        os << "add_custom_target(ASSIMPCopy\n";
        os << "COMMAND ${CMAKE_COMMAND} -E copy " + fixPath("../../external") + "/assimp/bin/assimp-vc142-mtd.dll ${CMAKE_BINARY_DIR}/Debug\n";
        os << "COMMAND ${CMAKE_COMMAND} -E copy " + fixPath("../../external") + "/assimp/bin/assimp-vc142-mtd.dll ${CMAKE_BINARY_DIR}/Release)\n";
        os << "add_dependencies(" << projectData->projectName << " ASSIMPCopy)\n";
        os << "target_compile_definitions(" << projectData->projectName << " PRIVATE TWE_BUILD=\"" << "./" + projectData->projectName + ".build" << "\")\n";

        os.close();
    }

    void BuildCreator::createBuildFile(ProjectData* projectData, const std::filesystem::path& buildFilePath, const std::filesystem::path& startScenePath) {
        nlohmann::json jsonMain;

        jsonMain["Build"] = projectData->projectName;
        jsonMain["ProjectFilePath"] =  projectData->projectName + ".project";
        jsonMain["StartScenePath"] = startScenePath.string(); 
    
        File::save(buildFilePath.string().c_str(), jsonMain.dump());
    }

    BuildData* BuildCreator::load(const std::filesystem::path& buildFilePath) {
        if(!std::filesystem::exists(buildFilePath))
            return nullptr;
        std::string jsonBodyStr = File::getBody(buildFilePath.string().c_str());
        nlohmann::json jsonMain = nlohmann::json::parse(jsonBodyStr);
        return new BuildData(jsonMain["Build"], jsonMain["ProjectFilePath"], jsonMain["StartScenePath"]);
    }

    std::string BuildCreator::fixPath(const std::string& path) {
        std::string res = std::filesystem::absolute(path).string();
        std::replace(res.begin(), res.end(), '\\', '/');
        return res;
    }
}