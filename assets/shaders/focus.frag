#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 Color;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    FragColor = Color;
}