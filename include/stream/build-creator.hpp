#ifndef BUILD_CREATOR_HPP
#define BUILD_CREATOR_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <single_include/nlohmann/json.hpp>

#include "stream/file.hpp"
#include "stream/project-creator.hpp"

namespace TWE {
    struct BuildData {
        BuildData() = default;
        BuildData(const std::string& buildName, const std::filesystem::path& projectFilePath, const std::filesystem::path& startScenePath)
        : buildName(buildName), projectFilePath(projectFilePath), startScenePath(startScenePath) {}
        std::string buildName;
        std::filesystem::path projectFilePath;
        std::filesystem::path startScenePath;
    };

    class BuildCreator {
    public:
        static bool create(ProjectData* projectData, const std::filesystem::path& startScenePath);
        static BuildData* load(const std::filesystem::path& buildFilePath);
    private:
        static void copyRootPathFiles(ProjectData* projectData, const std::filesystem::path& buildDirPath);
        static void createCMakeFile(ProjectData* projectData, const std::filesystem::path& buildDirPath);
        static void createBuildFile(ProjectData* projectData, const std::filesystem::path& buildFilePath, const std::filesystem::path& startScenePath);
        static std::string fixPath(const std::string& path);
    };
}

#endif