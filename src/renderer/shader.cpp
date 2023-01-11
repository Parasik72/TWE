#include "renderer/shader.hpp"

namespace TWE {
    Shader::Shader(const char* vertPath, const char* fragPath)
    : _vertPath(vertPath), _fragPath(fragPath) {
        std::string vertBody = File::getBody(vertPath);
        std::string fragBody = File::getBody(fragPath);
        const char* vertex = vertBody.c_str();
        const char* fragment = fragBody.c_str();
        uint32_t vertexShader = create(vertex, GL_VERTEX_SHADER);
        uint32_t fragmentShader = create(fragment, GL_FRAGMENT_SHADER);
        _id = glCreateProgram();
        uint32_t shaders[2] = {
            vertexShader,
            fragmentShader
        };
        linkShaders(shaders, 2);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::Shader(const Shader& shader) {
        this->_id = shader._id;
        this->_vertPath = shader._vertPath;
        this->_fragPath = shader._fragPath;
    }

    Shader::~Shader() {
        clean();
    }

    uint32_t Shader::create(const char* fileContent, uint32_t shaderType) {
        uint32_t shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &fileContent, nullptr);
        glCompileShader(shader);
        int status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!status) {
            char errorInfo[512];
            glGetShaderInfoLog(shader, sizeof(errorInfo), NULL, errorInfo);
            std::cout << "Error compilation a shader.\n" << errorInfo << std::endl;
        };
        return shader;
    }

    void Shader::linkShaders(uint32_t* shaders, uint32_t size) {
        for(int i = 0; i < size; ++i)
            glAttachShader(_id, shaders[i]);
        glLinkProgram(_id);
        int status;
        glGetProgramiv(_id, GL_COMPILE_STATUS, &status);
        if(!status) {
            char errorInfo[512];
            glGetProgramInfoLog(_id, sizeof(errorInfo), NULL, errorInfo);
            std::cout << "Error link a shader.\n" << errorInfo << std::endl;
        };
    }

    void Shader::use() {
        glUseProgram(_id);
    }

    void Shader::clean() {
        glDeleteProgram(_id);
    }

    void Shader::setUniform(const char* name, const glm::mat4& mat) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setUniform(const char* name, const glm::vec3& vec) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniform3f(loc, vec.x, vec.y, vec.z);
    }

    void Shader::setUniform(const char* name, float value) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniform1f(loc, value);
    }

    void Shader::setUniform(const char* name, uint32_t value) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniform1ui(loc, value);
    }

    void Shader::setUniform(const char* name, int value) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniform1i(loc, value);
    }

    void Shader::setUniform(const char* name, bool value) {
        use();
        uint32_t loc = glGetUniformLocation(getId(), name);
        glUniform1i(loc, value);    
    }

    uint32_t Shader::getId() const noexcept { return _id; }
    std::string Shader::getVertPath() const noexcept { return _vertPath; }
    std::string Shader::getFragPath() const noexcept { return _fragPath; }

    const char* TRANS_MAT_OPTIONS[3] = {
        "model",
        "view",
        "projection"
    };

    #ifndef TWE_BUILD
    const char* SHADER_PATHS[6] = {
        "../../shaders/default.vert",
        "../../shaders/default.frag",
        "../../shaders/cubemap.vert",
        "../../shaders/cubemap.frag",
        "../../shaders/collider.vert",
        "../../shaders/collider.frag",
    };
    #else
    const char* SHADER_PATHS[6] = {
        "./shaders/default.vert",
        "./shaders/default.frag",
        "./shaders/cubemap.vert",
        "./shaders/cubemap.frag",
        "./shaders/collider.vert",
        "./shaders/collider.frag",
    };
    #endif
}