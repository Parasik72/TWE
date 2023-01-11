#ifndef VAO_HPP
#define VAO_HPP

#include <integer.hpp>
#include <glad.h>

#include "vbo.hpp"

namespace TWE {
    class VAO {
    public:
        VAO();
        VAO(const VAO& vao);
        ~VAO();
        void bind();
        void unbind();
        void clean();
        void setAttrib(uint32_t layout, int size, uint32_t type, bool normalized, int step, const void* offset, VBO& vbo);
        [[nodiscard]] uint32_t getId();
    private:
        uint32_t _id;
    };
}

#endif