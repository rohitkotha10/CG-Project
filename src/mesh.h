#pragma once
#include "primitives.h"

namespace CG {
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        /**
         * @brief the indices for drawing triangles
         */
        std::vector<GLuint> indices;
        /**
         * @brief textures to apply on mesh
         */
        std::vector<Texture> textures;
        glm::vec4 color;
        /**
         * @brief set up data for passing into OpenGL shaders
         * @param vertices
         * @param indices
         * @param textures
         * @param color
         */
        void createMesh(
            std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, glm::vec4 color);
        /**
         * @brief generate a square on the xz plane at with center z
         * @param center
         * @param side
         * @param texture
         * @param texScale how much factor the texture must be minimized to for the entire plane
         */
        void createPlaneTexture(glm::vec3 center, float side, Texture texture, float texScale);
        /**
         * @brief generate a square on the xz plane at with center z
         * @param center
         * @param side
         * @param color
         */
        void createPlaneColor(glm::vec3 center, float side, glm::vec4 color);
        /**
         * @brief draw all the vertices as triangles with help of glDrawElements
         * @param program the shaders to use for drawing
         */
        void draw(Program& program);
        /**
         * @brief clean up vertex arrays and buffers
         */
        void shutdown();

    private:
        /**
         * @brief vertex array object
         */
        GLuint vao;
        /**
         * @brief vertex buffer object
         */
        GLuint vbo;
        /**
         * @brief element buffer object
         */
        GLuint ebo;
    };
}  // namespace CG
