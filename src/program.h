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
    /**
     * @brief parse the shader files and store as string
     * @param shaderPath source directory
     * @return a string containing the whole shader program
     */
    std::string parse(std::string shaderPath);

    class Program {
    public:
        /**
         * @brief compile the shaders, link them and create a program
         * @param vsSource source directory of vertex shader
         * @param fsSource source directory of fragment shader
         */
        void create(std::string vsSource, std::string fsSource);
        /**
         * @brief use the program and draw the shaders accordingly
         */
        void use();
        /**
         * @brief clean up the program
         */
        void shutdown();
        /**
         * @brief set int in uniform
         * @param name uniform name
         * @param value
         */
        void setInt(std::string name, int value);
        /**
         * @brief set float in uniform
         * @param name uniform name
         * @param value
         */
        void setFloat(std::string name, float value);
        /**
         * @brief set 4x4 matrix in uniform
         * @param name uniform name
         * @param data pass data as glm::mat4
         */
        void setMat4(std::string name, const glm::mat4& data);
        /**
         * @brief set vec4 matrix using 4 floats
         * @param name uniform name
         * @param x
         * @param y
         * @param z
         * @param a
         */
        void setVec4(std::string name, float x, float y, float z, float a);
        /**
         * @brief set vec4 using glm::vec4
         * @param name uniform name
         * @param data
         */
        void setVec4(std::string name, const glm::vec4& data);
        /**
         * @brief set vec3 using 3 floats
         * @param name uniform name
         * @param x
         * @param y
         * @param z
         */
        void setVec3(std::string name, float x, float y, float z);
        /**
         * @brief set vec3 using glm::vec3
         * @param name uniform name
         * @param data
         */
        void setVec3(std::string name, const glm::vec3& data);

    private:
        /**
         * @brief id returned from glCreate
         */
        GLuint id;
    };
}  // namespace CG
