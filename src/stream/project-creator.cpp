#include "stream/project-creator.hpp"

namespace TWE {
    bool ProjectCreator::create(const std::string& projectName, const std::string& projectPath) {
        if(!validateProjectName(projectName))
            return false;
        if(!std::filesystem::exists(projectPath))
            return false;
        std::string projectDirectoryPath = projectPath + '/' + projectName;
        std::filesystem::create_directory(projectDirectoryPath);
        std::filesystem::create_directory(projectDirectoryPath + "/models");
        std::filesystem::create_directory(projectDirectoryPath + "/scenes");
        std::filesystem::create_directory(projectDirectoryPath + "/textures");
        std::filesystem::create_directory(projectDirectoryPath + "/scripts");
        std::filesystem::create_directory(projectDirectoryPath + "/scripts/temp");
        createProjectFile(projectName, projectDirectoryPath);
        return true;
    }

    ProjectData* ProjectCreator::load(const std::string& projectFilePath, Registry<DLLLoadData>* scriptDLLRegistry) {
        if(!std::filesystem::exists(projectFilePath))
            return nullptr;
        std::string jsonBodyStr = File::getBody(projectFilePath.c_str());
        nlohmann::json jsonMain = nlohmann::json::parse(jsonBodyStr);
        auto& scripts = jsonMain["ScriptsDLL"].items();
        std::filesystem::path rootPath;
        #ifndef TWE_BUILD
        rootPath = (std::string)jsonMain["ProjectRootPath"];
        #else
        rootPath = "./";
        #endif
        for(auto& [index, data] : scripts) {
            DLLLoadData* dllData = new DLLLoadData();
            deserializaScriptDLL(dllData, data, rootPath);
            scriptDLLRegistry->add(dllData->scriptName, dllData);
        }
        return new ProjectData(jsonMain["Project"], jsonMain["ProjectRootPath"], jsonMain["DLLTempDir"], jsonMain["LastScenePath"]);
    }

    bool ProjectCreator::save(ProjectData* projectData, Registry<DLLLoadData>* scriptDLLRegistry) {
        std::filesystem::path projectFilePath = projectData->rootPath.string() + '/' + projectData->projectName + ".project";
        if(!std::filesystem::exists(projectFilePath))
            return false;
        nlohmann::json jsonMain;
        jsonMain["Project"] = projectData->projectName;
        jsonMain["ProjectRootPath"] = projectData->rootPath;
        jsonMain["DLLTempDir"] = projectData->dllTempDir.string();
        std::string lastScenePath = std::filesystem::relative(projectData->lastScenePath, projectData->rootPath).string();
        jsonMain["LastScenePath"] = !lastScenePath.empty() ? lastScenePath : projectData->lastScenePath;
        nlohmann::json jsonScripts = nlohmann::json::array();
        auto& scriptDLLRegistrySource = scriptDLLRegistry->getSource();
        for(auto& [key, value] : scriptDLLRegistrySource) {
            if(!value)
                continue;
            nlohmann::json jsonScript;
            serializeScriptDLL(value, jsonScript, projectData);
            jsonScripts.push_back(jsonScript);
        }
        jsonMain["ScriptsDLL"] = jsonScripts;
        File::save(projectFilePath.string().c_str(), jsonMain.dump());
        return true;
    }

    void ProjectCreator::createProjectFile(const std::string& projectName, const std::string& projectDirectoryPath) {
        nlohmann::json jsonMain;
        std::filesystem::path projectFilePath = projectDirectoryPath + '/' + projectName + ".project";

        jsonMain["Project"] = projectName;
        jsonMain["ProjectRootPath"] = std::filesystem::absolute(projectDirectoryPath).string();
        jsonMain["DLLTempDir"] = "/scripts/temp";
        jsonMain["LastScenePath"] = "";
        jsonMain["ScriptsDLL"] = nlohmann::json::array();

        File::save(projectFilePath.string().c_str(), jsonMain.dump());
    }

    void ProjectCreator::serializeScriptDLL(DLLLoadData* dllData, nlohmann::json& jsonScript, ProjectData* projectData) {
        if(!dllData)
            return;
        jsonScript["DLLPath"] = std::filesystem::relative(dllData->dllPath, projectData->rootPath).string();
        jsonScript["FactoryFuncName"] = dllData->factoryFuncName;
        jsonScript["ScriptName"] = dllData->scriptName;
        jsonScript["ScriptDirectoryPath"] = std::filesystem::relative(dllData->scriptDirectoryPath, projectData->rootPath).string();
    }

    void ProjectCreator::deserializaScriptDLL(DLLLoadData* dllData, nlohmann::json& jsonScript, const std::filesystem::path& rootPath) {
        dllData->isValid = true;
        dllData->dllPath = rootPath.string() + '/' + (std::string)jsonScript["DLLPath"];
        dllData->factoryFuncName = jsonScript["FactoryFuncName"];
        dllData->scriptName = jsonScript["ScriptName"];
        dllData->scriptDirectoryPath = rootPath.string() + '/' + (std::string)jsonScript["ScriptDirectoryPath"];
    }

    bool ProjectCreator::validateProjectName(const std::string& projectName) {
        return std::regex_match(projectName, std::regex("^[a-zA-Z][a-zA-Z0-9]+"));
    }
}