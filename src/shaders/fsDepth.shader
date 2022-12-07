#version 450 core

out vec4 FragColor;

in vec4 fragPos;

void main() {
    float dep = fragPos.z / fragPos.w;
    // change from [-1, 1] to [0,1]
    dep = dep * 0.5 + 0.5;

    FragColor = vec4(vec3(dep), 1.0f);
};