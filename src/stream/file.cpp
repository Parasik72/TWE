#include "stream/file.hpp"

namespace TWE {
    std::string File::getBody(const char* filePath){
        std::ifstream is;
        is.open(filePath);
        if(!is.is_open()){
            std::cout << "A file by < " << filePath << " > path was not found." << std::endl;
            return {};
        }
        std::stringstream ss;
        ss << is.rdbuf();
        is.close();
        return ss.str();
    }

    void File::save(const char* filePath, const std::string& data) {
        std::fstream fs;
        fs.open(filePath, std::ios::out | std::ios::trunc);
        fs << data;
        fs.close();
    }

    void File::remove(const char* filePath) {
        if(!std::filesystem::exists(filePath))
            return;
        std::remove(filePath);
    }
}