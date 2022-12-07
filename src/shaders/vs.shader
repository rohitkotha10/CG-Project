#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexPos;

out vec3 fragPos;
out vec3 vNormal;
out vec2 vTexPos;
out vec4 fragInLight;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

uniform mat4 lightSpace;

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
    fragPos = vec3(model_matrix * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(model_matrix))) * aNormal;
    vTexPos = aTexPos;
    fragInLight = lightSpace * model_matrix * vec4(aPos, 1.0);
};