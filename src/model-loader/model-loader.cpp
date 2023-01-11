#include "model-loader/model-loader.hpp"

namespace TWE {
    void ModelLoader::clean() {
        meshes.clear();
        fileDir.clear();
        hasTextures = false;
    }

    ModelLoaderData* ModelLoader::loadModel(const std::string& path) {
        clean();
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if(!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cout << "Error model loading:\n" << importer.GetErrorString() << std::endl;
            return nullptr;
        }
        fileDir = path.substr(0, path.find_last_of('/'));
        procNode(scene->mRootNode, scene);
        return new ModelLoaderData(meshes, path, ShaderIndices::DEFAULT_VERT, ShaderIndices::DEFAULT_FRAG);
    }

    std::vector<std::string> ModelLoader::loadMatText(aiMaterial* mat, aiTextureType type) {
        std::vector<std::string> texturesPath;
        int texCount = mat->GetTextureCount(type);
        for(int i = 0; i < texCount; ++i) {
            aiString texturePath;
            mat->GetTexture(type, i, &texturePath);
            std::string fullPath = fileDir + '/' + texturePath.C_Str();
            texturesPath.push_back(fullPath);
            if(!hasTextures)
                hasTextures = true;
        }
        return texturesPath;
    }

    MeshComponent ModelLoader::procMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<GLuint> indices;
        int vertSize = mesh->mNumVertices * 8;
        float* vertices = new float[vertSize];
        std::vector<std::string> texturesPath;
        int verticesIndex = 0;
        for(int i = 0; i < mesh->mNumVertices; ++i){
            //vertex
            if(!mesh->mVertices)
                continue;
            vertices[verticesIndex++] = mesh->mVertices[i].x;
            vertices[verticesIndex++] = mesh->mVertices[i].y;
            vertices[verticesIndex++] = mesh->mVertices[i].z;
            //normals
            vertices[verticesIndex++] = mesh->mNormals ? mesh->mNormals[i].x : 0.f;
            vertices[verticesIndex++] = mesh->mNormals ? mesh->mNormals[i].y : 0.f;
            vertices[verticesIndex++] = mesh->mNormals ? mesh->mNormals[i].z : 0.f;
            //texture coords
            if( mesh->mTextureCoords[0] ) {
                vertices[verticesIndex++] = mesh->mTextureCoords[0][i].x;
                vertices[verticesIndex++] = mesh->mTextureCoords[0][i].y;
            }
            else {
                vertices[verticesIndex++] = 0.f;
                vertices[verticesIndex++] = 0.f;
            }
        }
        for(int i = 0; i < mesh->mNumFaces; ++i){
            auto face = mesh->mFaces[i];
            for(int j = 0 ; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        TextureAttachmentSpecification textureAtttachments;
        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<std::string> diff = loadMatText(material, aiTextureType_DIFFUSE);
            for(auto& imgPath : diff) {
                TextureSpecification textureSpecification;
                textureSpecification.imgPath = imgPath;
                textureSpecification.texNumber = 0;
                textureSpecification.texType = TextureType::Texture2D;
                textureSpecification.inOutTexFormat = TextureInOutFormat::RGBA;
                textureAtttachments.textureSpecifications.push_back(textureSpecification);
            }
            texturesPath.insert(texturesPath.end(), diff.begin(), diff.end());
        }
        int indSize = indices.size();
        uint32_t* copyIndices = new uint32_t[indSize];
        for(int i = 0; i < indSize; ++i)
            copyIndices[i] = indices[i];
        return MeshComponent(vertices, vertSize * sizeof(GLfloat), copyIndices, indSize * sizeof(GLuint), "Model mesh", textureAtttachments);
    }

    void ModelLoader::procNode(aiNode* node, const aiScene* scene) {
        for(int i = 0; i < node->mNumMeshes; ++i){
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(procMesh(mesh, scene));
        }
        for(int i = 0; i < node->mNumChildren; ++i)
            procNode(node->mChildren[i], scene);
    }
}