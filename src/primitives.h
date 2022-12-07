#pragma once
#include "program.h"

#include <stb_image/stb_image.h>

namespace CG {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texPos;
    };

    struct Texture {
        GLuint id;
        std::string type;
        std::string path;
        void loadTexture(std::string path, std::string typeName);
    };
}  // namespace CG
