#version 330 core

layout (location = 0) in vec3 pos;

out vec3 texCoord;

uniform mat4 view = mat4(1.0f);
uniform mat4 projection = mat4(1.0f);
uniform mat4 model = mat4(1.0f);

void main() {
   gl_Position = projection * mat4(mat3(view)) * model * vec4(pos, 1.0f);
   texCoord = pos;
}