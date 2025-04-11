#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in uvec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor = vec4(inColor.r, inColor.g, inColor.b, inColor.a) / 255.0;
}