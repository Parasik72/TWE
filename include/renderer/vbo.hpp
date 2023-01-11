#ifndef VBO_HPP
#define VBO_HPP

#include <integer.hpp>
#include <glad.h>

namespace TWE {
    class VBO {
    public:
        VBO() = default;
        VBO(const VBO& vbo);
        VBO(float* vertices, long int size, uint32_t drawType);
        ~VBO();
        void bind();
        void unbind();
        void clean();
        [[nodiscard]] uint32_t getId();
        [[nodiscard]] float* getVertices();
        [[nodiscard]] long int getSize();
    private:
        uint32_t _id;
        float* _vertices;
        long int _size;
    };
}

#endif