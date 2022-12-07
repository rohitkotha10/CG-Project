#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace CG {
    std::string parse(std::string shaderPath);

    class Program {
    public:
        void create(std::string vsSource, std::string fsSource);
        void use();
        void shutdown();
        void setInt(std::string name, int value);
        void setFloat(std::string name, float value);
        void setMat4(std::string name, const glm::mat4& data);
        void setVec4(std::string name, float x, float y, float z, float a);
        void setVec4(std::string name, const glm::vec4& data);
        void setVec3(std::string name, float x, float y, float z);
        void setVec3(std::string name, const glm::vec3& data);

    private:
        GLuint id;
    };
}  // namespace CG
