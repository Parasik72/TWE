#version 330 core

in vec3 texCoord;

layout (location = 0) out vec4 color;
layout (location = 1) out int outId;

uniform samplerCube textureImg;
uniform int id = -1;

void main(){
    color = texture(textureImg, texCoord);
    outId = -1;
}