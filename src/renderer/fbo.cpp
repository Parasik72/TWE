#include "renderer/fbo.hpp"

namespace TWE {
    FBO::FBO(uint32_t width, uint32_t height, const FBOAttachmentSpecification& attachments)
    : _width(width), _height(height), _attachments(attachments) {
        for(auto& specification : _attachments.textureSpecifications) {
            if(specification.textureFormat == FBOTextureFormat::DEPTH24STENCIL8)
                _depthSpecification = specification;
            else
                _colorSpecifications.push_back(specification);
        }
        create();
    }

    FBO::~FBO() {
        clean();
    }

    void FBO::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, _id);
        glViewport(0, 0, _width, _height);
    }

    void FBO::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FBO::clean() {
        glDeleteFramebuffers(1, &_id);
        glDeleteTextures(_colorAttachments.size(), _colorAttachments.data());
        glDeleteTextures(1, &_depthAttachment);
        _colorAttachments.clear();
        _depthAttachment = 0;
    }

    void FBO::resize(uint32_t width, uint32_t height) {
        _width = width;
        _height = height;
        clean();
        create();
    }

    void FBO::attachColorTexture(uint32_t id, uint32_t index, uint32_t inFormat, uint32_t outFormat, uint32_t width, uint32_t height) {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, outFormat, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void FBO::attachDepthTexture(uint32_t id, uint32_t inFormat, uint32_t outFormat, uint32_t width, uint32_t height) {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, outFormat, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void FBO::create() {
        glGenFramebuffers(1, &_id);
        bind();
        if(!_colorSpecifications.empty()) {
            _colorAttachments.resize(_colorSpecifications.size());
            glGenTextures(_colorAttachments.size(), _colorAttachments.data());
            int colorAttachmentsSize = _colorAttachments.size();
            for(int i = 0; i < colorAttachmentsSize; ++i) {
                switch (_colorSpecifications[i].textureFormat) {
                case FBOTextureFormat::R32I:
                    attachColorTexture(_colorAttachments[i], i, GL_R32I, GL_RED_INTEGER, _width, _height);
                    break;
                case FBOTextureFormat::RGBA8:
                    attachColorTexture(_colorAttachments[i], i, GL_RGBA8, GL_RGBA, _width, _height);
                    break;
                }
            }
        }
        if(_depthSpecification.textureFormat != FBOTextureFormat::None) {
            glGenTextures(1, &_depthAttachment);
            switch (_depthSpecification.textureFormat) {
                case FBOTextureFormat::DEPTH24STENCIL8:
                    attachDepthTexture(_depthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, _width, _height);
                    break;
            }
        }
        if(_colorAttachments.size() > 1) {
            uint32_t buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(_colorAttachments.size(), buffers);
        } else if(_colorAttachments.empty())
            glDrawBuffer(GL_NONE);
        unbind();
    }

    int FBO::readPixel(uint32_t colorAttachmentId, uint32_t x, uint32_t y) {
        int result;
        bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentId);
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &result);
        unbind();
        return result;
    }

    uint32_t FBO::getId() const noexcept { return _id; }
    std::pair<uint32_t, uint32_t> FBO::getSize() const noexcept { return {_width, _height}; }
    FBOAttachmentSpecification& FBO::getAttachments() { return _attachments; }
    uint32_t FBO::getColorAttachment(uint32_t index) const noexcept { return _colorAttachments[index]; }
    uint32_t FBO::getDepthAttachment() const noexcept { return _depthAttachment; }
}