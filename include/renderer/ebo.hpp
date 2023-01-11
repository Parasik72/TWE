#ifndef EBO_HPP
#define EBO_HPP

#include <integer.hpp>
#include <glad.h>

namespace TWE {
    class EBO {
    public:
        EBO() = default;
        EBO(const EBO& ebo);
        EBO(uint32_t* indices, long int size, uint32_t drawType);
        ~EBO();
        void bind();
        void unbind();
        void clean();
        [[nodiscard]] uint32_t getId();
        [[nodiscard]] uint32_t* getIndices();
        [[nodiscard]] long int getSize();
    private:
        uint32_t _id;
        uint32_t* _indices;
        long int _size;
    };
}

#endif