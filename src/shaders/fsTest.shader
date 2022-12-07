#version 450 core

out vec4 FragColor;

in vec2 vTexPos;

uniform sampler2D myTex;

void main() {
    float depthValue = texture(myTex, vTexPos).r;
    FragColor = vec4(vec3(depthValue), 1.0);
};