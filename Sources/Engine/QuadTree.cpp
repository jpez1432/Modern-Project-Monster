

#include "QuadTree.hpp"

using namespace Engine;

CNode::CNode(void) : Leaf(false)//, GLTerrain(0), GLBoxes(0)
{

    for(int i = 0; i < 4; i++) {
        Child[i] = NULL;
    }

}

CNode::~CNode(void)
{
    if(Leaf) {

        for(unsigned int i = 0; i < TerrainChunks.size(); i++) {
            glDeleteVertexArrays(1, &TerrainChunks[i].VAO);
            TerrainChunks[i].Data.Destroy();
            TerrainChunks[i].Indices.Destroy();
        }

        for(unsigned int i = 0; i < BoxChunks.size(); i++) {
            glDeleteVertexArrays(1, &BoxChunks[i].VAO);
            BoxChunks[i].Data.Destroy();
            BoxChunks[i].Indices.Destroy();
        }


    }
}

void CNode::Render(bool Invert)
{
//    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    for(unsigned int i = 0; i < TerrainChunks.size(); i++) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TerrainChunks[i].Texture);

        glBindVertexArray(TerrainChunks[i].VAO);
        glDrawElements(GL_QUADS, TerrainChunks[i].Count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

    for(unsigned int i = 0; i < BoxChunks.size(); i++) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, BoxChunks[i].Texture);

        glBindVertexArray(BoxChunks[i].VAO);
        glDrawElements(GL_QUADS, BoxChunks[i].Count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

}

CQuadTree::CQuadTree(void) : Root(NULL), Dimensions(256), LeafSize(32)
{


}

CQuadTree::~CQuadTree(void)
{
    Destroy();
}

CNode *CQuadTree::CreateNode(glm::vec3 Bound[4])
{
    int OffsetX, OffsetZ;
    int Index, Rotation;
    int MirrorX, MirrorY;
    float Lower, Upper;

    glm::vec2 Coord(0);
    glm::vec3 Vertex(0);
    glm::vec3 Normal(0);
    std::vector<unsigned int> Duplicates;

    CNode *Node = new CNode();
    Node->Center = (glm::vec3(Bound[3] - Bound[0]) * 0.5f + Bound[0]);
    unsigned int Size = Bound[1].x - Bound[0].x;

    for(int i = 0; i < 4; i++) {
        Node->Bounds[i] = Bound[i];
    }


    if(Size == LeafSize) {

        Node->Leaf = true;

        Game::CTerrain *Terrain = Track->TerrainObject();
        Game::CTexturePool *Pool = Track->TexturePoolObject();
        Game::CBoxes *Boxes = Track->BoxesObject();

        for(unsigned int x = 0; x < LeafSize; x++) {
            for(unsigned int z = 0; z < LeafSize; z++) {

                OffsetX = (Node->Center.x - LeafSize / 2) + x;
                OffsetZ = (Node->Center.z - LeafSize / 2) + z;
                Duplicates.push_back(Terrain->GetTextureIndex(OffsetX, OffsetZ));

            }
        }

        std::sort(Duplicates.begin(), Duplicates.end());
        Duplicates.erase(std::unique(Duplicates.begin(), Duplicates.end()), Duplicates.end());

        Node->TerrainChunks.resize(Duplicates.size());

        for(unsigned int i = 0; i < Node->TerrainChunks.size(); i++) {

            Node->TerrainChunks[i].Texture = Pool->GetTexture(Duplicates[i]);

            Node->TerrainChunks[i].Count = 0;
            Node->TerrainChunks[i].Data.Create();
            Node->TerrainChunks[i].Indices.Create();

            for(unsigned int x = 0; x < LeafSize; x++) {
                for(unsigned int z = 0; z < LeafSize; z++) {

                    OffsetX = (Node->Center.x - LeafSize / 2) + x;
                    OffsetZ = (Node->Center.z - LeafSize / 2) + z;

                    if(Terrain->GetTextureIndex(OffsetX, OffsetZ) == (int)Duplicates[i]) {

                        Index = Terrain->GetTextureRotation(OffsetX, OffsetZ);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = Terrain->GetHeightScaled(OffsetX, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Node->TerrainChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->TerrainChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->TerrainChunks[i].Data.AddData(&Terrain->GetNormal(OffsetX, OffsetZ), sizeof(glm::vec3));
                        Node->TerrainChunks[i].Indices.AddData(&Node->TerrainChunks[i].Count, sizeof(int));
                        Node->TerrainChunks[i].Count++;

                        Vertex = Terrain->GetHeightScaled(OffsetX + 1, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Node->TerrainChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->TerrainChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->TerrainChunks[i].Data.AddData(&Terrain->GetNormal(OffsetX + 1, OffsetZ), sizeof(glm::vec3));
                        Node->TerrainChunks[i].Indices.AddData(&Node->TerrainChunks[i].Count, sizeof(int));
                        Node->TerrainChunks[i].Count++;

                        Vertex = Terrain->GetHeightScaled(OffsetX + 1, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Node->TerrainChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->TerrainChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->TerrainChunks[i].Data.AddData(&Terrain->GetNormal(OffsetX + 1, OffsetZ + 1), sizeof(glm::vec3));
                        Node->TerrainChunks[i].Indices.AddData(&Node->TerrainChunks[i].Count, sizeof(int));
                        Node->TerrainChunks[i].Count++;

                        Vertex = Terrain->GetHeightScaled(OffsetX, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Node->TerrainChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->TerrainChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->TerrainChunks[i].Data.AddData(&Terrain->GetNormal(OffsetX, OffsetZ + 1), sizeof(glm::vec3));
                        Node->TerrainChunks[i].Indices.AddData(&Node->TerrainChunks[i].Count, sizeof(int));
                        Node->TerrainChunks[i].Count++;

                    }
                }
            }

            glGenVertexArrays(1, &Node->TerrainChunks[i].VAO);
            glBindVertexArray(Node->TerrainChunks[i].VAO);

            Node->TerrainChunks[i].Data.Bind();
            Node->TerrainChunks[i].Data.DataToGPU(GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

            Node->TerrainChunks[i].Indices.Bind(GL_ELEMENT_ARRAY_BUFFER);
            Node->TerrainChunks[i].Indices.DataToGPU(GL_STATIC_DRAW);

        }

        Duplicates.clear();
        for(unsigned int x = 0; x < LeafSize; x++) {
            for(unsigned int z = 0; z < LeafSize; z++) {
                for(unsigned int i = 0; i < 6; i++) {
                    OffsetX = (Node->Center.x - LeafSize / 2) + x;
                    OffsetZ = (Node->Center.z - LeafSize / 2) + z;
                    Duplicates.push_back(Boxes->GetTextureIndex(OffsetX, OffsetZ, i));
                }
            }
        }

        std::sort(Duplicates.begin(), Duplicates.end());
        Duplicates.erase(std::unique(Duplicates.begin(), Duplicates.end()), Duplicates.end());

        Node->BoxChunks.resize(Duplicates.size());

        for(unsigned int i = 0; i < Node->BoxChunks.size(); i++) {

            Node->BoxChunks[i].Texture = Pool->GetTexture(Duplicates[i]);

            Node->BoxChunks[i].Count = 0;
            Node->BoxChunks[i].Data.Create();
            Node->BoxChunks[i].Indices.Create();

            for(unsigned int x = 0; x < LeafSize; x++) {
                for(unsigned int z = 0; z < LeafSize; z++) {

                    for (unsigned int j = 0; j < 6; j++) {

                    OffsetX = (Node->Center.x - LeafSize / 2) + x;
                    OffsetZ = (Node->Center.z - LeafSize / 2) + z;

                    if(Boxes->GetTextureIndex(OffsetX, OffsetZ, j) == (int)Duplicates[i]) {

                    Lower = Boxes->GetLowerHeight(OffsetX, OffsetZ);
                    Upper = Boxes->GetUpperHeight(OffsetX, OffsetZ);

                    if(Lower != Upper) {

                        switch (j) {

                        case 0:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 0);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        break;

                        case 1:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 1);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(-1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(-1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(-1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(-1, 0, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        break;

                        case 2:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 2);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(0, 0, -1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(0, 0, -1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(0, 0, -1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(0, 0, -1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        break;

                        case 3:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 3);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(0, 0, 1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(0, 0, 1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(0, 0, 1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(0, 0, 1);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        break;

                        case 4:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 4);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(0, -1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(0, -1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(0, -1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX, Upper, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(0, -1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        break;

                        case 5:

                        Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 5);
                        MirrorX = (Index & 32) >> 5;
                        MirrorY = (Index & 16) >> 4;
                        Rotation = (Index & 192) >> 6;

                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
                        Normal = glm::vec3(0, 1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;

                        Vertex = glm::vec3(OffsetX, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
                        Normal = glm::vec3(0, 1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ + 1);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
                        Normal = glm::vec3(0, 1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;


                        Vertex = glm::vec3(OffsetX + 1, Lower, OffsetZ);
                        Coord = glm::vec2(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
                        Normal = glm::vec3(0, 1, 0);
                        Node->BoxChunks[i].Data.AddData(&Vertex, sizeof(glm::vec3));
                        Node->BoxChunks[i].Data.AddData(&Coord, sizeof(glm::vec2));
                        Node->BoxChunks[i].Data.AddData(&Normal, sizeof(glm::vec3));
                        Node->BoxChunks[i].Indices.AddData(&Node->BoxChunks[i].Count, sizeof(int));
                        Node->BoxChunks[i].Count++;
                            break;
                        }
                    }
                }
            }
            }

            }
            glGenVertexArrays(1, &Node->BoxChunks[i].VAO);
            glBindVertexArray(Node->BoxChunks[i].VAO);

            Node->BoxChunks[i].Data.Bind();
            Node->BoxChunks[i].Data.DataToGPU(GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

            Node->BoxChunks[i].Indices.Bind(GL_ELEMENT_ARRAY_BUFFER);
            Node->BoxChunks[i].Indices.DataToGPU(GL_STATIC_DRAW);
        }

    } else {

        glm::vec3 B[4];

        for(int i = 0; i < 4; i++) {

            GetBoundingBox(B, Bound[0], Size / 2.0f, i);
            Node->Child[i] = CreateNode(B);

        }

    }

    return Node;

}

void CQuadTree::Destroy(void)
{
    if(Root) {
        CloseNode(Root);
        Root = NULL;
    }
}

void CQuadTree::GetBoundingBox(glm::vec3 Out[4], glm::vec3 Offset, int Boxsize, int I)
{

    glm::vec3 Shift;

    if(I == 1) {
        Shift = glm::vec3(Boxsize, 0.0f, 0.0f);

    } else if(I == 2) {
        Shift = glm::vec3(0.0f, 0.0f, Boxsize);

    } else if(I == 3) {
        Shift = glm::vec3(Boxsize, 0.0f, Boxsize);
    }

    Out[0] = Offset	+ Shift;
    Out[1] = Offset + glm::vec3(Boxsize, 0.0f, 0.0f) + Shift;
    Out[2] = Offset + glm::vec3(0.0f, 0.0f, Boxsize) + Shift;
    Out[3] = Offset + glm::vec3(Boxsize, 0.0f, Boxsize) + Shift;

}

void CQuadTree::CloseNode(CNode *Node)
{

    for(int i = 0; i < 4; i++) {

        if(Node->Child[i]) {

            CloseNode(Node->Child[i]);
            delete Node->Child[i];
            Node->Child[i] = NULL;

        }

    }

}

bool CQuadTree::Create(Game::CTrack *Track, unsigned int LeafSize)
{

    this->Track = Track;
    this->LeafSize = LeafSize;
    this->Dimensions = Track->Dimensions();

    glm::vec3 Bounds[4];
    GetBoundingBox(Bounds, glm::vec3(0.0f, 0.0f, 0.0f), Dimensions, 0);

    Root = CreateNode(Bounds);

    if(Root) {
        return true;
    }

    return false;

}

//
//#include "QuadTree.hpp"
//
//using namespace Engine;
//
//CNode::CNode(void) : Leaf(false), Reflects(false), GLTerrain(0), GLBoxes(0)
//{
//    for (int i = 0; i < 4; i++) {
//        Child[i] = NULL;
//    }
//}
//
//CNode::~CNode(void)
//{
//    if (Leaf) {
//        glDeleteLists(GLTerrain, 1);
//        glDeleteLists(GLBoxes, 1);
//    }
//}
//
//void CNode::Render(bool Invert)
//{
//    glPushMatrix();
//    glEnable(GL_CLIP_PLANE0);
//
//    if (Invert) {
//
//        glScalef(1.0f, -2.0f, 1.0f);
//
//        glCullFace(GL_FRONT);
//
//        if (Config->Render.Terrain && Reflects) {
//            glCallList(GLTerrain);
//        }
//
//        if (Config->Render.Boxes) {
//            glCallList(GLBoxes);
//        }
//
//    } else {
//
//        GLdouble PlaneEqn[] = { 0.0f, 1.0f, 0.0f, 0.0f };
//        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);
//
//        glCullFace(GL_BACK);
//
//        if (Config->Render.Terrain) {
//            glCallList(GLTerrain);
//        }
//
//        if (Config->Render.Boxes) {
//            glCallList(GLBoxes);
//        }
//
//    }
//
//    glDisable(GL_CLIP_PLANE0);
//    glPopMatrix();
//
//}
//
//CQuadTree::CQuadTree(void) : Root(NULL), Dimensions(256), LeafSize(32)
//{
//
//
//}
//
//CQuadTree::~CQuadTree(void)
//{
//    Destroy();
//}
//
//CNode *CQuadTree::CreateNode(glm::vec3 Bound[4])
//{
//
//    CNode *Node = new CNode();
//    Node->Center = (glm::vec3(Bound[3] - Bound[0]) * 0.5f + Bound[0]);
//    unsigned int Size = Bound[1].x - Bound[0].x;
//
//    for (int i = 0; i < 4; i++) {
//        Node->Bounds[i] = Bound[i];
//    }
//
//
//
//    if (Size == LeafSize) {
//
//        Node->Leaf = true;
//
//        int Index, Rotation;
//        int MirrorX, MirrorY;
//        int OffsetX, OffsetZ;
//        float Lower, Upper;
//
//        std::vector<unsigned int> Duplicates;
//        Game::CTexturePool *Pool = Track->TexturePoolObject();
//        Game::CTerrain *Terrain = Track->TerrainObject();
//        Game::CBoxes *Boxes = Track->BoxesObject();
//
//        for (unsigned int x = 0; x < LeafSize; x++) {
//            for (unsigned int z = 0; z < LeafSize; z++) {
//
//                OffsetX = (Node->Center.x - LeafSize / 2) + x;
//                OffsetZ = (Node->Center.z - LeafSize / 2) + z;
//                Duplicates.push_back(Terrain->GetTextureIndex(OffsetX, OffsetZ));
//
//                if (Terrain->GetNormal(OffsetX, OffsetZ) != glm::vec3(0.0f, -1.0f, 0.0f)) {
//                    Node->Reflects = true;
//                }
//
//            }
//        }
//
//        std::sort(Duplicates.begin(), Duplicates.end());
//        Duplicates.erase(std::unique(Duplicates.begin(), Duplicates.end()), Duplicates.end());
//
//
//        Node->GLTerrain = glGenLists(1);
//        glNewList(Node->GLTerrain, GL_COMPILE);
//
//        glDisable(GL_BLEND);
//        glDisable(GL_ALPHA_TEST);
//
//        for (unsigned int i = 0; i < Duplicates.size(); i++) {
//
//            Pool->Bind(Duplicates[i]);
//
//            glBegin(GL_QUADS);
//
//            for (unsigned int x = 0; x < LeafSize; x++) {
//                for (unsigned int z = 0; z < LeafSize; z++) {
//
//                    OffsetX = (Node->Center.x - LeafSize / 2) + x;
//                    OffsetZ = (Node->Center.z - LeafSize / 2) + z;
//
//                    if (Terrain->GetTextureIndex(OffsetX, OffsetZ) == int(Duplicates[i])) {
//
//                        Index = Terrain->GetTextureRotation(OffsetX, OffsetZ);
//                        MirrorX = (Index & 32) >> 5;
//                        MirrorY = (Index & 16) >> 4;
//                        Rotation = (Index & 192) >> 6;
//
//                        glNormal3fv(&Terrain->GetNormal(OffsetX, OffsetZ)[0]);
//                        glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                        glVertex3f(OffsetX, Terrain->GetHeightScaled(OffsetX, OffsetZ), OffsetZ);
//
//                        glNormal3fv(&Terrain->GetNormal(OffsetX + 1, OffsetZ)[0]);
//                        glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                        glVertex3f(OffsetX + 1, Terrain->GetHeightScaled(OffsetX + 1, OffsetZ), OffsetZ);
//
//                        glNormal3fv(&Terrain->GetNormal(OffsetX + 1, OffsetZ + 1)[0]);
//                        glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                        glVertex3f(OffsetX + 1, Terrain->GetHeightScaled(OffsetX + 1, OffsetZ + 1), OffsetZ + 1);
//
//                        glNormal3fv(&Terrain->GetNormal(OffsetX, OffsetZ + 1)[0]);
//                        glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                        glVertex3f(OffsetX, Terrain->GetHeightScaled(OffsetX, OffsetZ + 1), OffsetZ + 1);
//
//
//                    }
//
//                }
//            }
//
//            glEnd();
//
//        }
//
//        glEndList();
//
//
//        Node->GLBoxes = glGenLists(1);
//        glNewList(Node->GLBoxes, GL_COMPILE);
//
//        for (unsigned int x = 0; x < LeafSize; x++) {
//            for (unsigned int z = 0; z < LeafSize; z++) {
//
//                OffsetX = (Node->Center.x - LeafSize / 2) + x;
//                OffsetZ = (Node->Center.z - LeafSize / 2) + z;
//
//                Lower = Boxes->GetLowerHeight(OffsetX, OffsetZ);
//                Upper = Boxes->GetUpperHeight(OffsetX, OffsetZ);
//
//                if (Lower != Upper) {
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 0);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 0));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ);
//
//                    glNormal3f(1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ);
//
//                    glNormal3f(1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ + 1);
//
//                    glNormal3f(1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ + 1);
//
//                    glEnd();
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 1);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 1));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(-1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
//
//                    glNormal3f(-1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
//
//                    glNormal3f(-1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ);
//
//                    glNormal3f(-1, 0, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ);
//
//                    glEnd();
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 2);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 2));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(0, 0, -1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ + 1);
//
//                    glNormal3f(0, 0, -1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ + 1);
//
//                    glNormal3f(0, 0, -1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
//
//                    glNormal3f(0, 0, -1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
//
//                    glEnd();
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 3);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 3));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(0, 0, 1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ);
//
//                    glNormal3f(0, 0, 1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ);
//
//                    glNormal3f(0, 0, 1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ);
//
//                    glNormal3f(0, 0, 1);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ);
//
//                    glEnd();
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 4);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 4));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(0, -1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ);
//
//                    glNormal3f(0, -1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ);
//
//                    glNormal3f(0, -1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
//
//                    glNormal3f(0, -1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX, Upper, OffsetZ + 1);
//
//                    glEnd();
//
//                    Index = Boxes->GetTextureRotation(OffsetX, OffsetZ, 5);
//                    MirrorX = (Index & 32) >> 5;
//                    MirrorY = (Index & 16) >> 4;
//                    Rotation = (Index & 192) >> 6;
//
//                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 5));
//                    glBegin(GL_QUADS);
//
//                    glNormal3f(0, 1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ);
//
//                    glNormal3f(0, 1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3, 1));
//                    glVertex3f(OffsetX, Lower, OffsetZ + 1);
//
//                    glNormal3f(0, 1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
//
//                    glNormal3f(0, 1, 0);
//                    glTexCoord2f(Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 0), Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1, 1));
//                    glVertex3f(OffsetX + 1, Lower, OffsetZ);
//
//                    glEnd();
//
//                }
//
//            }
//        }
//
//        glEndList();
//
//    } else {
//
//        glm::vec3 B[4];
//
//        for (int i = 0; i < 4; i++) {
//
//            GetBoundingBox(B, Bound[0], Size / 2.0f, i);
//            Node->Child[i] = CreateNode(B);
//
//        }
//
//    }
//
//    return Node;
//
//}
//
//void CQuadTree::Destroy(void)
//{
//    if (Root) {
//        CloseNode(Root);
//        Root = NULL;
//    }
//}
//
//void CQuadTree::GetBoundingBox(glm::vec3 Out[4], glm::vec3 Offset, int Boxsize, int I)
//{
//
//    glm::vec3 Shift;
//
//    if (I == 1) {
//        Shift = glm::vec3(Boxsize, 0.0f, 0.0f);
//
//    } else if (I == 2) {
//        Shift = glm::vec3(0.0f, 0.0f, Boxsize);
//
//    } else if (I == 3) {
//        Shift = glm::vec3(Boxsize, 0.0f, Boxsize);
//    }
//
//    Out[0] = Offset	+ Shift;
//    Out[1] = Offset + glm::vec3(Boxsize, 0.0f, 0.0f) + Shift;
//    Out[2] = Offset + glm::vec3(0.0f, 0.0f, Boxsize) + Shift;
//    Out[3] = Offset + glm::vec3(Boxsize, 0.0f, Boxsize) + Shift;
//
//}
//
//void CQuadTree::CloseNode(CNode *Node)
//{
//
//    for (int i = 0; i < 4; i++) {
//
//        if (Node->Child[i]) {
//
//            CloseNode(Node->Child[i]);
//            delete Node->Child[i];
//            Node->Child[i] = NULL;
//
//        }
//
//    }
//
//}
//
//bool CQuadTree::Create(Game::CTrack *Track, unsigned int LeafSize)
//{
//
//    this->Track = Track;
//    this->LeafSize = LeafSize;
//    this->Dimensions = Track->Dimensions();
//
//    glm::vec3 Bounds[4];
//    GetBoundingBox(Bounds, glm::vec3(0.0f, 0.0f, 0.0f), Dimensions, 0);
//
//    Root = CreateNode(Bounds);
//
//    if (Root) {
//        return true;
//    }
//
//    return false;
//
//}
//
//
//
//
//
//
//
//
//
//
