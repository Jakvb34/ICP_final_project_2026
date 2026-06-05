#version 410 core

layout(location = 0) in vec3 attribute_Position;
layout(location = 1) in vec3 attribute_Normal;
layout(location = 2) in vec2 attribute_TexCoords;

uniform mat4 uP_m = mat4(1.0);
uniform mat4 uV_m = mat4(1.0);
uniform mat4 uM_m = mat4(1.0);

out VS_OUT {
    vec3 N;
    vec3 P;
    vec3 V;
    vec2 texCoord;
} vs_out;

void main() {
    mat4 mv_m = uV_m * uM_m;

    vec4 P_view = mv_m * vec4(attribute_Position, 1.0);

    vs_out.P = P_view.xyz;
    vs_out.N = normalize(mat3(transpose(inverse(mv_m))) * attribute_Normal);
    vs_out.V = normalize(-P_view.xyz);
    vs_out.texCoord = attribute_TexCoords;

    gl_Position = uP_m * P_view;
}