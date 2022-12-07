#pragma once
#include "primitives.h"

namespace CG {
    class Framebuffer {
    public:
        void create(int scrw, int scrh);
        void shutdown();
        void drawfbo(Program& program);  // draw the texture on a quad

        GLuint id;
        GLuint texture;
        GLuint rbo;

        GLuint quadvao;
        GLuint quadvbo;
        GLuint quadebo;
    };
}  // namespace CG
