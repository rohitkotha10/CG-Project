#pragma once
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CG {
    class Importer {
    public:
        /**
         * @brief process the model from path and triangulate it using assimp
         * @param path source directory
         */
        void loadModel(std::string path);
        /**
         * @brief draw all the meshes in the model
         * @param program the shader program to use for rendering
         */
        void draw(Program& program);
        /**
         * @brief clean up all the meshes in the model
         */
        void shutdown();

    private:
        /**
         * @brief the model is split into many meshes
         */
        std::vector<Mesh> meshes;
        /**
         * @brief source directory
         */
        std::string directory;
        /**
         * @brief to avoid loading same texture multiple times, use this
         */
        std::vector<Texture> textures_loaded;

        /**
         * @brief start processing node recursively
         * @param node each node has set of child nodes and a set of meshes in it
         * @param scene the source for our whole model generated by assimp
         */
        void processNode(aiNode* node, const aiScene* scene);
        /**
         * @brief generate vertices, textures, color and save it
         * @param mesh the mesh generated by assimp
         * @param scene the source for our whole model generated by assimp
         * @return mesh
         */
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        /**
         * @brief load textures based on type
         * @param mat the assimp material
         * @param type the type to search for in assimp
         * @param typeName can be only diffuse or specular
         * @return array of textures to be applied on the mesh
         */
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        /**
         * @brief load the diffuse color only from material
         * @param mat the assimp material
         * @return the color vector
         */
        glm::vec4 loadColor(aiMaterial* mat);
    };
}  // namespace CG
