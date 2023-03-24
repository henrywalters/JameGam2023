#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    FragColor = texture(image, TexCoord);
}