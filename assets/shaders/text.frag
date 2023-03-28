#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 TextColor;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    float alpha = texture(image, TexCoord).r;
    FragColor = vec4(TextColor.rgb, alpha);
}