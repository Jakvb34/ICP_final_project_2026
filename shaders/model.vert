#version 410 core

layout(location = 0) in vec3 attribute_Position;
layout(location = 1) in vec3 attribute_Normal;
layout(location = 2) in vec2 attribute_TexCoords;

uniform mat4 uP_m = mat4(1.0);
uniform mat4 uV_m = mat4(1.0);
uniform mat4 uM_m = mat4(1.0);

out vec2 texcoord;

void main() {
    gl_Position = uP_m * uV_m * uM_m * vec4(attribute_Position, 1.0);
    texcoord = attribute_TexCoords;
}