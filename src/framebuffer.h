#pragma once
#include "primitives.h"

namespace CG {
    class Framebuffer {
    public:
        /**
         * @brief create a frame buffer texture with depth and stencil buffer enabled and also a quad to display the
         * framebuffer on screen
         * @param scrw screen width
         * @param scrh screen height
         */
        void create(int scrw, int scrh);
        /**
         * @brief clean up the framebuffer and the quad
         */
        void shutdown();
        /**
         * @brief draw the framebuffer texture on the window
         * @param program the shader to use for drawing framebuffer texture
         */
        void drawfbo(Program& program);
        /**
         * @brief id returned from glCreate
         */
        GLuint id;
        /**
         * @brief framebuffer texture
         */
        GLuint texture;
        /**
         * @brief depth and stencil buffer stored in the render buffer object
         */
        GLuint rbo;

        /**
         * @brief quad vertex array object
         */
        GLuint quadvao;
        /**
         * @brief quad vertex buffer object
         */
        GLuint quadvbo;
        /**
         * @brief quad element buffer object
         */
        GLuint quadebo;
    };
}  // namespace CG
