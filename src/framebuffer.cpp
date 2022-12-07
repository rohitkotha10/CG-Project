#include "framebuffer.h"

using namespace std;

namespace CG {
    void Framebuffer::create(int scrw, int scrh) {
        glGenFramebuffers(1, &this->id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->id);

        glGenRenderbuffers(1, &this->rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrw, scrh);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

        glGenTextures(1, &this->texture);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrw, scrh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Error: Incomplete framebuffer" << endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        Vertex a0;
        a0.position = glm::vec3(-1.0f, -1.0f, 0.0f);
        a0.texPos = glm::vec2(0.0f, 0.0f);

        Vertex a1;
        a1.position = glm::vec3(-1.0f, 1.0f, 0.0f);
        a1.texPos = glm::vec2(0.0f, 1.0f);

        Vertex a2;
        a2.position = glm::vec3(1.0f, 1.0f, 0.0f);
        a2.texPos = glm::vec2(1.0f, 1.0f);

        Vertex a3;
        a3.position = glm::vec3(1.0f, -1.0f, 0.0f);
        a3.texPos = glm::vec2(1.0f, 0.0f);

        vector<Vertex> vertices = {a0, a1, a2, a3};
        vector<int> indices = {0, 1, 2, 2, 3, 0};

        glGenVertexArrays(1, &quadvao);
        glGenBuffers(1, &quadvbo);
        glGenBuffers(1, &quadebo);

        glBindVertexArray(quadvao);

        glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texPos));
        glEnableVertexAttribArray(1);
    }

    void Framebuffer::drawfbo(Program& program) {
        program.use();
        glBindVertexArray(this->quadvao);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Framebuffer::shutdown() {
        glDeleteFramebuffers(1, &this->id);
        glDeleteRenderbuffers(1, &this->rbo);
        glDeleteTextures(1, &this->texture);
    }
}  // namespace CG
