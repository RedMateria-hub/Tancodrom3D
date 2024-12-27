#pragma once
#include <vector>
#include <iostream>
#include "Texture.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    vector<string> nodeNames;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool bSmoothNormals, bool gamma = false);

    // draws the model, and thus all its meshes
    virtual void Draw(Shader& shader);

    void printNodeNames();

    void SetRootTransf(glm::mat4 transform);
    void setNodeTransforms(std::string st, glm::mat4 mat);

    void setIsAnimated(bool animated);

    void moveModel(glm::mat4 transform);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path, bool bSmoothNormals);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(std::string nodeName, aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    // Store transformations for specific nodes
    std::map<std::string, glm::mat4> nodeTransforms;
    bool isAnimated = false;
};

