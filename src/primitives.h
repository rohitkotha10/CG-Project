#pragma once
#include "program.h"

#include <stb_image/stb_image.h>

namespace CG {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        /**
         * @brief texture co-ordinates for uv mapping
         */
        glm::vec2 texPos;
    };

    struct Texture {
        /**
         * @brief id returned from glCreate
         */
        GLuint id;
        /**
         * @brief type of texture for shading
         */
        std::string type;
        /**
         * @brief source directory of the texture
         */
        std::string path;
        /**
         * @brief load a texture and generate id from opengl
         * @param path source directory
         * @param typeName only diffuse and specular maps are supported
         */
        void loadTexture(std::string path, std::string typeName);
    };
}  // namespace CG
