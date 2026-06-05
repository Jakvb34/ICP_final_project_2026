#version 410 core

in vec2 texcoord;

uniform sampler2D tex0;

out vec4 FragColor;

void main() {
    FragColor = texture(tex0, texcoord);
}