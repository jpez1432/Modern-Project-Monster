

#ifndef MODELS_HPP
#define MODELS_HPP

#include <algorithm>

#include "../Engine/VBO.hpp"
#include "Pods.hpp"
#include "Textures.hpp"

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CModel
{

private:

    struct {
        int Scale;
        int Unknown1;
        int Unknown2;
        int NumVertices;
    } Header;

    struct AnimatedTexture {
        int NumTextures;
        int Delay;
        std::vector<std::string>Filenames;
    };

    struct VertInfo {
        int Index;
        glm::vec2 TexCoord;
    };

    struct Face {
        int Type;
        int TextureIndex;
        std::vector<VertInfo>Vertices;
    };

    struct Chunk {
        GLuint Indices;
        GLuint Transparent;
        GLuint Texture;
        GLuint VAO;
        Engine::CVBO Data;
        Engine::CVBO DataIndices;
    };

    std::vector<Chunk> Chunks;
    std::vector<glm::vec3> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> FaceNormals;
    std::vector<std::string> Textures;
    std::vector<AnimatedTexture> AnimatedTextures;
    std::vector<glm::vec4> Colors;
    std::vector<Face> Faces;

    int ModelID;
    float BoundingSphere;
    glm::vec3 Min, Max;

    GLuint GLModel;
    Game::CTexturePool TexturePool;
    std::string Filename;

    bool Build(void);
    void CalculateNormals(void);

public:

    CModel(void);
    ~CModel(void);

    bool Load(CPodPool &PodPool, std::string Filename);
    void Destroy(void);

    void Render(void);

    float CalculateBounds(glm::vec3 &Min, glm::vec3 &Max);
    glm::vec3 GetBounds(glm::vec3 &Min, glm::vec3 &Max);

    const float GetBoundingSphere(void)
    {
        return BoundingSphere;
    }
    const std::string GetFilename(void)
    {
        return Filename;
    }
    const GLuint GetModel(void)
    {
        return GLModel;
    }

};

class CModelPool
{

private:

    std::vector<CModel> Models;

public:

    CModelPool(void);
    ~CModelPool(void);

    bool Create(CPodPool &PodPool, std::vector<std::string> &Filenames);

    const float GetBoundingSphere(int Index)
    {
        return Models[Index].GetBoundingSphere();
    }
    const glm::vec3 GetBounds(int Index, glm::vec3 &Min, glm::vec3 &Max)
    {
        return Models[Index].GetBounds(Min, Max);
    }
    void GetModel(int Index)
    {
        Models[Index].Render();
//        return Models[Index].GetModel();
    }
    const std::string Filename(int Index)
    {
        return Models[Index].GetFilename();
    }
    const int NumModels(void)
    {
        return Models.size();
    }

};

}

#endif



//
//#ifndef MODELS_HPP
//#define MODELS_HPP
//
//#include "../Common.hpp"
//#include "Pods.hpp"
//#include "Textures.hpp"
//
//namespace Game {
//
//    class CModel {
//
//        private:
//
//            struct {
//                int Scale;
//                int Unknown1;
//                int Unknown2;
//                int NumVertices;
//            } Header;
//
//            struct Texture {
//                char Filename[16];
//            };
//
//            struct AnimatedTexture {
//                int NumTextures;
//                int Delay;
//                std::vector<std::string>Filenames;
//            };
//
//            struct VertInfo {
//                int Index;
//                glm::i32vec2 TexCoord;
//            };
//
//            struct Face {
//                int Type;
//                int TextureIndex;
//                int NumVertices;
//                glm::i32vec3 Normal;
//                int Magic;
//                std::vector<VertInfo>Vertices;
//            };
//
//            std::vector<glm::i32vec3> Vertices;
//            std::vector<glm::i32vec3> Normals;
//            std::vector<glm::vec3> FaceNormals;
//            std::vector<glm::vec3> VerticeNormals;
//            std::vector<Texture> Textures;
//            std::vector<AnimatedTexture> AnimatedTextures;
//            std::vector<glm::i8vec4> Colors;
//            std::vector<Face> Faces;
//
//            bool UseNormals;
//            int ModelID;
//            float BoundingSphere;
//            glm::vec3 Min, Max;
//
//            GLuint GLModel;
//            Game::CTexturePool TexturePool;
//            std::string Filename;
//
//            bool BuildList(void);
//            void CalculateNormals(void);
//            Face &CalculateFaceNormal(Face &face);
//
//        public:
//
//            CModel(void);
//            ~CModel(void);
//
//            bool Load(CPodPool &PodPool, std::string Filename);
//
//            glm::vec3 GetDimensions(glm::vec3 &Min, glm::vec3 &Max);
//            const float GetBoundingSphere(void) { return BoundingSphere; }
//            const std::string GetFilename(void) { return Filename; }
//            const GLuint GetModel(void) { return GLModel; }
//
//    };
//
//    class CModelPool {
//
//        private:
//
//            std::vector<CModel> Models;
//
//        public:
//
//            CModelPool(void);
//            ~CModelPool(void);
//
//            bool Create(CPodPool &PodPool, std::vector<std::string> &Filenames);
//
//            const int GetBoundingSphere(int Index) { return Models[Index].GetBoundingSphere(); }
//            const GLuint GetModel(int Index) { return Models[Index].GetModel(); }
//            const std::string Filename(int Index) { return Models[Index].GetFilename(); }
//            const int NumModels(void) { return Models.size(); }
//
//    };
//
//}
//
//#endif
//
