#include "mesh.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>

class Model 
{
    public:
        /*  函数   */
        void loadModel(std::string const &path);
        void Draw(unsigned int shaderProgram, int amount=0);
        void setupInstanceMatrices(int amount, glm::mat4* modelMatrices);   
    private:
        /*  模型数据  */
        std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes;
        std::string directory;
        /*  函数   */
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
};