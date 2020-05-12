
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>
#include <string>

#include "Pods.hpp"

namespace Game
{

class CTerrain
{

private:

    int Dimensions;
    float MinHeight;

    typedef struct {
        unsigned char Index;
        unsigned char Rotation;
    } Information;

    std::vector<Information> TextureInfo;
    std::vector<glm::vec3> HeightMap;
    std::vector<glm::vec3> Normals;

    std::vector<unsigned char> HeightmapData;

    void CalculateNormals(void);

    const int IndexAt(int X, int Z);

public:

    CTerrain(void);
    ~CTerrain(void);

    bool Load(CPodPool &PodPool, std::string Filename, std::string Info);

    const int GetDimensions(void)
    {
        return Dimensions;
    }

    const int GetTextureIndex(int X, int Z);
    const int GetTextureRotation(int X, int Z);

    const glm::vec3 GetHeight(int X, int Z, float Scale = 16.0f);
    glm::vec3 GetHeightScaled(int X, int Z);
    glm::vec3 &GetNormal(int X, int Z);

    const unsigned char &GetHeightData(void)
    {
        return HeightmapData[0];
    }

};

}

#endif
