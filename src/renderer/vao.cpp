#include "renderer/vao.hpp"

namespace TWE {
    VAO::VAO() {
        glGenVertexArrays(1, &_id);
    }

    VAO::VAO(const VAO& vao) {
        this->_id = vao._id;
    }

    VAO::~VAO() {
        clean();
    }

    void VAO::setAttrib(uint32_t layout, int size, uint32_t type, bool normalized, int step, const void* offset, VBO& vbo) {
        vbo.bind();
        glVertexAttribPointer(layout, size, type, normalized, step, offset);
        glEnableVertexAttribArray(layout);
        vbo.unbind();
    }

    void VAO::bind() {
        glBindVertexArray(_id);
    }

    void VAO::unbind() {
        glBindVertexArray(0);
    }

    void VAO::clean() {
        glDeleteVertexArrays(1, &_id);
    }

    uint32_t VAO::getId() { return _id; }
}