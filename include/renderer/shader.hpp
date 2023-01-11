#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad.h>
#include <gtc/type_ptr.hpp>
#include <string>

#include "stream/file.hpp"

namespace TWE {
    enum TransformMatrixOptions {
        MODEL,
        VIEW,
        PROJECTION
    };

    enum ShaderIndices {
        DEFAULT_VERT,
        DEFAULT_FRAG,
        CUBEMAP_VERT,
        CUBEMAP_FRAG,
        COLLIDER_VERT,
        COLLIDER_FRAG,
    };

    extern const char* TRANS_MAT_OPTIONS[3];

    extern const char* SHADER_PATHS[6];

    class Shader {
    public:
        Shader() = default;
        Shader(const Shader& shader);
        Shader(const char* vertPath, const char* fragPath);
        ~Shader();
        void use();
        void clean();
        void setUniform(const char* name, const glm::mat4& mat);
        void setUniform(const char* name, const glm::vec3& vec);
        void setUniform(const char* name, float value);
        void setUniform(const char* name, uint32_t value);
        void setUniform(const char* name, int value);
        void setUniform(const char* name, bool value);
        [[nodiscard]] uint32_t getId() const noexcept;
        [[nodiscard]] std::string getVertPath() const noexcept;
        [[nodiscard]] std::string getFragPath() const noexcept;
    private:
        [[nodiscard]] uint32_t create(const char* fileContent, uint32_t shaderType);
        void linkShaders(uint32_t* shaders, uint32_t size);
        uint32_t _id;
        std::string _vertPath;
        std::string _fragPath;
    };
}

#endif