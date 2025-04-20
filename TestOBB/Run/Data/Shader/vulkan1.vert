#version 450

layout(binding = 0) uniform ubo_model{
	mat4 model;
};

layout(binding = 1) uniform ubo_camera{
	mat4 proj;
	mat4 view;
};


layout(location = 0) in vec3 inPosition;
layout(location = 1) in uvec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = proj * view * model * vec4(inPosition, 1.0);
    fragColor = vec4(inColor.r, inColor.g, inColor.b, inColor.a) / 255.0;
    //fragTexCoord = inTexCoord;
}