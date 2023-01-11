#ifndef FBO_HPP
#define FBO_HPP

#include <glad.h>
#include <memory>
#include <vector>

namespace TWE {
    enum class FBOTextureFormat {
        None,
        R32I,
        RGBA8,
        DEPTH24STENCIL8
    };

    struct FBOTextureSpecification {
        FBOTextureSpecification() = default;
        FBOTextureSpecification(FBOTextureFormat textureFormat): textureFormat(textureFormat) {}
        FBOTextureFormat textureFormat = FBOTextureFormat::None;
    };

    struct FBOAttachmentSpecification {
        FBOAttachmentSpecification() = default;
        FBOAttachmentSpecification(std::initializer_list<FBOTextureSpecification> textureSpecifications)
            : textureSpecifications(textureSpecifications){}
        std::vector<FBOTextureSpecification> textureSpecifications;
    };

    class FBO {
    public:
        FBO(uint32_t width, uint32_t height, const FBOAttachmentSpecification& attachments);
        ~FBO();
        void bind();
        void unbind();
        void clean();
        void resize(uint32_t width, uint32_t height);
        [[nodiscard]] int readPixel(uint32_t colorAttachmentId, uint32_t x, uint32_t y);
        [[nodiscard]] FBOAttachmentSpecification& getAttachments();
        [[nodiscard]] uint32_t getId() const noexcept;
        [[nodiscard]] uint32_t getColorAttachment(uint32_t index) const noexcept;
        [[nodiscard]] uint32_t getDepthAttachment() const noexcept;
        [[nodiscard]] std::pair<uint32_t, uint32_t> getSize() const noexcept;
    private:
        void create();
        void attachColorTexture(uint32_t id, uint32_t index, uint32_t inFormat, uint32_t outFormat, uint32_t width, uint32_t height);
        void attachDepthTexture(uint32_t id, uint32_t inFormat, uint32_t outFormat, uint32_t width, uint32_t height);
        uint32_t _id;
        uint32_t _width;
        uint32_t _height;
        FBOAttachmentSpecification _attachments;
        std::vector<FBOTextureSpecification> _colorSpecifications;
        FBOTextureSpecification _depthSpecification;
        std::vector<uint32_t> _colorAttachments;
        uint32_t _depthAttachment;
    };
}

#endif