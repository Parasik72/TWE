#ifndef SCENE_BULLET_DEBUG_DRAWER_HPP
#define SCENE_BULLET_DEBUG_DRAWER_HPP

#include <glad.h>
#include <bullet3/LinearMath/btIDebugDraw.h>

#include "renderer/vao.hpp"
#include "renderer/vbo.hpp"
#include "renderer/ebo.hpp"
#include "renderer/shader.hpp"

namespace TWE {
    class SceneBulletDebugDrawer: public btIDebugDraw {
    public:
        SceneBulletDebugDrawer() {
            _shader = new Shader(SHADER_PATHS[ShaderIndices::COLLIDER_VERT], SHADER_PATHS[ShaderIndices::COLLIDER_FRAG]);
        }

        void setMats(const glm::mat4& view, const glm::mat4& projection) {
            _shader->setUniform(TRANS_MAT_OPTIONS[TransformMatrixOptions::PROJECTION], projection);
            _shader->setUniform(TRANS_MAT_OPTIONS[TransformMatrixOptions::VIEW], view);
        }
        
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
            GLfloat points[12] = {
                from.x(), from.y(), from.z(), color.x(), color.y(), color.z(),
                to.x(),   to.y(),   to.z(),   color.x(), color.y(), color.z(),
            };

		    glDeleteBuffers(1, &_VBO);
		    glDeleteVertexArrays(1, &_VAO);
		    glGenBuffers(1, &_VBO);
		    glGenVertexArrays(1, &_VAO);
            _shader->use();
		    glBindVertexArray(_VAO);
		    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		    glEnableVertexAttribArray(0);
		    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		    glEnableVertexAttribArray(1);
		    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		    glBindVertexArray(0);

		    glBindVertexArray(_VAO);
		    glDrawArrays(GL_LINES, 0, 2);
		    glBindVertexArray(0);
        }
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}
        void reportErrorWarning(const char* warningString) override {}
        void draw3dText(const btVector3& location, const char* textString) override {}
        void setDebugMode(int debugMode) override { _debugMode = debugMode; }
        int getDebugMode() const override { return _debugMode; }
    private:
        GLuint _VBO, _VAO;
        Shader* _shader;
        int _debugMode;
    };
}

#endif