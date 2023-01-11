#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalVecs;
layout (location = 2) in vec2 textureCoord;

out vec3 normal;
out vec2 texCoord;
out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 model = mat4(1.0f);
uniform mat4 view = mat4(1.0f);
uniform mat4 projection = mat4(1.0f);

void main() {
   gl_Position = projection * view * model * vec4(pos, 1.0);
   fragPos = vec3(model * vec4(pos, 1.0f));
   normal = mat3(transpose(inverse(model))) * normalVecs;
   texCoord = textureCoord;
}