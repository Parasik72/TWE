#ifndef DLL_CREATOR_HPP
#define DLL_CREATOR_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define TWE_PLATFORM_WINDOWS
#include "windows.h"
#else
#define TWE_DLL_CREATOR_ERROR
#endif

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "stream/file.hpp"

namespace TWE {
    typedef void*(*PVoid)();

    struct DLLLoadData {
        DLLLoadData() = default;
        DLLLoadData(const std::string& dllPath, const std::string& factoryFuncName, const std::string& scriptName, const std::string& scriptDirectoryPath)
            : dllPath(dllPath), factoryFuncName(factoryFuncName), scriptName(scriptName), scriptDirectoryPath(scriptDirectoryPath), isValid(true) {}
        bool isValid = false;
        #ifdef TWE_PLATFORM_WINDOWS
        std::vector<HINSTANCE> hDlls;
        #endif
        std::string dllPath;
        std::string factoryFuncName;
        std::string scriptName;
        std::string scriptDirectoryPath;
    };

    class DLLCreator {
    public:
        static DLLLoadData compileScript(const std::string& scriptName, const std::string& scriptDirectoryPath);
        static void initPaths(const std::string& tempDir);
        static void removeScript(const std::string& scriptName);
        [[nodiscard]] static PVoid loadDLLFunc(DLLLoadData& loadData);
        [[nodiscard]] static void freeDLLFunc(DLLLoadData& loadData);
    private:
        static std::string _tempDir;
        static void createScriptDirectory(const std::string& tempDir, const std::string& scriptName);
        static void createCMakeFile(const std::string& tempDir, const std::string& scriptName);
        static void createCPPFile(const std::string& tempDir, const std::string& scriptName, const std::string& scriptDirectoryPath);
        static std::string fixPath(const std::string& path);
    };
}

#endif