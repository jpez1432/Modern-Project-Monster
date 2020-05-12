

#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <vector>

#include "Frustum.hpp"
#include "VBO.hpp"
#include "../Game/Track.hpp"
#include "../Game/Terrain.hpp"
#include "../Game/Textures.hpp"

namespace Engine
{

class CNode
{

public:

    CNode *Child[4];

    bool Leaf;

    glm::vec3 Center;
    glm::vec3 Bounds[4];

//    GLuint GLTerrain;
//    GLuint GLBoxes;

    struct Terrain {
        GLuint Count;
        GLuint Texture;
        GLuint VAO;
        CVBO Data;
        CVBO Indices;
    };

    struct Box {
        GLuint Count;
        GLuint Texture;
        GLuint VAO;
        CVBO Data;
        CVBO Indices;
    };

    std::vector<Terrain> TerrainChunks;
    std::vector<Box> BoxChunks;

public:

    CNode(void);
    ~CNode(void);

    void Render(bool Invert);

};

class CQuadTree
{

private:

    Game::CTrack *Track;
    CNode *Root;

    unsigned int Dimensions;
    unsigned int LeafSize;

    CNode *CreateNode(glm::vec3 Bound[4]);
    void CloseNode(CNode *Node);

    void GetBoundingBox(glm::vec3 Out[4], glm::vec3 Offset, int Boxsize, int I);

public:

    CQuadTree(void);
    ~CQuadTree(void);

    bool Create(Game::CTrack *Track, unsigned int LeafSize);
    void Destroy(void);

    CNode *GetRootNode(void)
    {
        return Root;
    }
    unsigned int GetLeafSize(void)
    {
        return LeafSize;
    }

};
}

#endif


//
//#ifndef QUADTREE_HPP
//#define QUADTREE_HPP
//
//#include "Frustum.hpp"
//#include "../Game/Track.hpp"
//#include "../Game/Terrain.hpp"
//#include "../Game/Textures.hpp"
//
//namespace Engine
//{
//
//class CNode
//{
//
//public:
//
//    CNode *Child[4];
//
//    bool Leaf;
//    bool Reflects;
//
//    glm::vec3 Center;
//    glm::vec3 Bounds[4];
//
//    GLuint GLTerrain;
//    GLuint GLBoxes;
//
//public:
//
//    CNode(void);
//    ~CNode(void);
//
//    void Render(bool Invert);
//
//};
//
//class CQuadTree
//{
//
//private:
//
//    Game::CTrack *Track;
//    CNode *Root;
//
//    int Dimensions;
//    unsigned int LeafSize;
//
//    CNode *CreateNode(glm::vec3 Bound[4]);
//    void CloseNode(CNode *Node);
//
//    void GetBoundingBox(glm::vec3 Out[4], glm::vec3 Offset, int Boxsize, int I);
//
//public:
//
//    CQuadTree(void);
//    ~CQuadTree(void);
//
//    bool Create(Game::CTrack *Track, unsigned int LeafSize);
//    void Destroy(void);
//
//    CNode *GetRootNode(void)
//    {
//        return Root;
//    }
//    unsigned int GetLeafSize(void)
//    {
//        return LeafSize;
//    }
//
//};
//}
//
//#endif
