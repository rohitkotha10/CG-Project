#include "mesh.h"

using namespace std;

namespace CG {
    void Mesh::createMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, glm::vec4 color) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->color = color;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texPos));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void Mesh::createPlaneTexture(glm::vec3 center, float side, Texture texture, float texScale) {
        float hside = side / 2;

        Vertex a0;
        a0.position = glm::vec3(center.x - hside, center.y, center.z - hside);
        a0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a0.texPos = glm::vec2(0.0f, 0.0f);

        Vertex a1;
        a1.position = glm::vec3(center.x + hside, center.y, center.z - hside);
        a1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a1.texPos = glm::vec2(texScale, 0.0f);

        Vertex a2;
        a2.position = glm::vec3(center.x + hside, center.y, center.z + hside);
        a2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a2.texPos = glm::vec2(texScale, texScale);

        Vertex a3;
        a3.position = glm::vec3(center.x - hside, center.y, center.z + hside);
        a3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a3.texPos = glm::vec2(0.0f, texScale);

        vector<Vertex> vertices = {a0, a1, a2, a3};
        vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
        vector<Texture> textures = {texture};

        this->createMesh(vertices, indices, textures, glm::vec4(1.0f));
    }

    void Mesh::createPlaneColor(glm::vec3 center, float side, glm::vec4 color) {
        float hside = side / 2;

        Vertex a0;
        a0.position = glm::vec3(center.x - hside, center.y, center.z - hside);
        a0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a0.texPos = glm::vec2(0.0f, 0.0f);

        Vertex a1;
        a1.position = glm::vec3(center.x + hside, center.y, center.z - hside);
        a1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a1.texPos = glm::vec2(1.0f, 0.0f);

        Vertex a2;
        a2.position = glm::vec3(center.x + hside, center.y, center.z + hside);
        a2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a2.texPos = glm::vec2(1.0f, 1.0f);

        Vertex a3;
        a3.position = glm::vec3(center.x - hside, center.y, center.z + hside);
        a3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        a3.texPos = glm::vec2(0.0f, 1.0f);

        vector<Vertex> vertices = {a0, a1, a2, a3};
        vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
        vector<Texture> textures = {};

        this->createMesh(vertices, indices, textures, color);
    }

    void Mesh::draw(Program& program) {
        int numDiffuse = 0;
        int numSpecular = 0;
        for (unsigned int i = 0; i < textures.size(); i++) {
            // allowing only 1 diffuse and 1 specular map for model

            string name = textures[i].type;
            if (name == "diffuse" && numDiffuse <= 1) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                program.setInt("material.diffuse", 0);
                numDiffuse++;
            } else if (name == "specular" && numSpecular <= 1) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                program.setInt("material.specular", 1);
                numSpecular++;
            } else {
                cout << "Unknown texture type: " << textures[i].path << endl;
            }
        }
        if (numSpecular > 1 || numDiffuse > 1) cout << "Only 1 diffuse and 1 specular supported!" << endl;

        program.setVec4("material.color", this->color);
        if (numDiffuse == 0) {
            program.setInt("material.hasDiffMap", 0);
            program.setInt("material.hasSpecMap", 0);
        } else {
            program.setInt("material.hasDiffMap", 1);
            if (numSpecular == 0)
                program.setInt("material.hasSpecMap", 0);
            else if (numSpecular == 1)
                program.setInt("material.hasSpecMap", 1);
        }

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::shutdown() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        for (int i = 0; i < textures.size(); i++) glDeleteTextures(1, &textures[i].id);
    }
}  // namespace CG
