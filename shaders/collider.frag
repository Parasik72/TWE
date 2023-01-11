#version 330 core

layout (location = 0) out vec4 outColor;

uniform float alpha = 1.f;

in vec3 inColor;

void main(){
    outColor = vec4(inColor, alpha);
}