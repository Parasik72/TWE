#include "renderer/vbo.hpp"

namespace TWE {
    VBO::VBO(float* vertices, long int size, uint32_t drawType)
    : _vertices(vertices), _size(size) {
        glGenBuffers(1, &_id);
        glBindBuffer(GL_ARRAY_BUFFER, _id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VBO::VBO(const VBO& vbo) {
        this->_id = vbo._id;
        this->_vertices = vbo._vertices;
        this->_size = vbo._size;
    }

    VBO::~VBO() {
        clean();
    }

    void VBO::bind(){
        glBindBuffer(GL_ARRAY_BUFFER, _id);
    }

    void VBO::unbind(){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VBO::clean(){
        glDeleteBuffers(1, &_id);
    }

    uint32_t VBO::getId() { return _id; }
    float* VBO::getVertices() { return _vertices; }
    long int VBO::getSize() { return _size; }
}