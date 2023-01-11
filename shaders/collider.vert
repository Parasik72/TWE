#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 view = mat4(1.0f);
uniform mat4 projection = mat4(1.0f);

out vec3 inColor;

void main() {
   gl_Position = projection * view * vec4(pos, 1.0f);
   inColor = color;
}