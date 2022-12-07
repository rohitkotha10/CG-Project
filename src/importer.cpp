#include "importer.h"

using namespace std;

namespace CG {
    void Importer::draw(Program& program) {
        for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].draw(program);
    }

    void Importer::loadModel(string path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Importer::processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++) { processNode(node->mChildren[i], scene); }
    }

    Mesh Importer::processMesh(aiMesh* mesh, const aiScene* scene) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        glm::vec4 color;

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            color = loadColor(material);

            for (int i = 0; i < diffuseMaps.size(); i++) textures.push_back(diffuseMaps[i]);

            for (int i = 0; i < specularMaps.size(); i++) textures.push_back(specularMaps[i]);
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // process vertex positions, normals and texture coordinates
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;

            if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texPos = vec;
            } else
                vertex.texPos = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
        }

        Mesh temp;
        temp.createMesh(vertices, indices, textures, color);
        return temp;
    }

    void Importer::shutdown() {
        for (int i = 0; i < meshes.size(); i++) { meshes[i].shutdown(); }
    }

    vector<Texture> Importer::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            string filename = string(str.C_Str());
            filename = directory + '/' + filename;

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (textures_loaded[j].path == filename) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip) {  // if texture hasn't been loaded already, load it
                Texture texture;
                texture.loadTexture(filename, typeName);
                textures_loaded.push_back(texture);  // add to loaded textures
                textures.push_back(texture);
            }
        }
        return textures;
    }

    glm::vec4 Importer::loadColor(aiMaterial* mat) {
        aiColor3D color(0.f, 0.f, 0.f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        return glm::vec4(color.r, color.g, color.b, 1.0f);
    }
}  // namespace CG
