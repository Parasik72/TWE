#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad.h>
#include <stb_image.h>
#include <string>
#include <algorithm>

#include "shader.hpp"
#include "fbo.hpp"

namespace TWE {
    enum class TextureType {
        Texture2D      = GL_TEXTURE_2D,
        CubemapTexture = GL_TEXTURE_CUBE_MAP
    };

    enum class TextureInOutFormat {
        RGBA = GL_RGBA
    };

    struct TextureSpecification {
        TextureSpecification() = default;
        TextureSpecification(const std::string& imgPath, uint32_t texNumber, TextureType texType, TextureInOutFormat inOutTexFormat)
            : imgPath(imgPath), texType(texType), texNumber(texNumber), inOutTexFormat(inOutTexFormat){}
        uint32_t id;
        std::string imgPath;
        uint32_t texNumber;
        TextureType texType;
        TextureInOutFormat inOutTexFormat;
    };

    struct TextureAttachmentSpecification {
        TextureAttachmentSpecification() = default;
        TextureAttachmentSpecification(std::initializer_list<TextureSpecification> textureSpecifications)
            : textureSpecifications(textureSpecifications){}
        std::vector<TextureSpecification> textureSpecifications;
    };

    class Texture {
    public:
        Texture() = default;
        Texture(const Texture& texture);
        Texture(const TextureAttachmentSpecification& attachments);
        ~Texture();
        void bind();
        void unbind();
        void clean();
        void setAttachments(const TextureAttachmentSpecification& attachments);
        void setTexture(TextureSpecification& textureSpec);
        void removeTexture(uint32_t texNumber);
        [[nodiscard]] uint32_t getId(int index) const noexcept;
        [[nodiscard]] uint32_t getIndexByTexNumber(uint32_t texNumber) const noexcept;
        [[nodiscard]] TextureSpecification* getTextureSpecByTexNumber(uint32_t texNumber);
        [[nodiscard]] TextureAttachmentSpecification& getAttachments();
        static Texture* generateCubemapTexture(TextureAttachmentSpecification& attachments);
    private:
        void create(TextureSpecification& textureSpecification);
        TextureAttachmentSpecification _attachments;
    };
}

#endif