

#include "Models.hpp"

using namespace Game;

CModel::CModel(void)
{

}

CModel::~CModel(void)
{
    Destroy();
}

bool CModel::Load(CPodPool &PodPool, std::string Filename)
{

    int Size, Offset;
    std::string File;

    int BlockID = -1;
    int X, Y;
    int NumVertices = 0;
    int NumTextures = 0;
    int TextureIndex = 0;
    char Texture[16];
    char AnimTexture[32];
    glm::i32vec3 Vertex;

    glm::i8vec4 Color;
    Face Split, Triangle;
    Triangle.Vertices.resize(3);

    this->Filename = Filename;

    Filename.insert(0, "Models\\");
    PodPool.FindFile(Filename, Size, Offset);

    if (Offset == 0 && !Config->Game.Files) {
        return false;
    }

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::binary | std::ios::in);

    if (StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, std::ios::beg);
    StreamIn.read((char*)&ModelID, sizeof(ModelID));

    if (ModelID == 0x20) {

        StreamIn.seekg(20, std::ios::cur);

        char Buffer[16];
        StreamIn.read((char*)&Buffer, sizeof(Buffer));

        Logger->Error("Animated Model, Loading First Frame - " + std::string(Buffer), true, false, false);

        Load(PodPool, Buffer);

        return true;

    } else if (ModelID != 0x14) {

        Logger->Error("Error Loading Model, Invalid Model ID Number - " + Filename);

    }

    StreamIn.read((char*)&Header, sizeof(Header));

    for (int i = 0; i < Header.NumVertices; i++) {
        StreamIn.read((char*)&Vertex[0], sizeof(glm::i32vec3));
        Vertices.push_back(glm::vec3(Vertex.x / 8192.0f, Vertex.y / 8192.0f, Vertex.z / -8192.0f));
    }

    StreamIn.read((char*)&BlockID, sizeof(BlockID));

    while (BlockID != 0 && !StreamIn.eof()) {

        switch (BlockID) {

        case 0x03:

            StreamIn.seekg(sizeof(glm::i32vec3) * Header.NumVertices + 8, std::ios::cur);
            break;

        case 0x17:

            StreamIn.seekg(8, std::ios::cur);
            break;

        case 0x0D:

            StreamIn.seekg(4, std::ios::cur);
            StreamIn.read((char*)&Texture[0], sizeof(char) * 16);
            TextureIndex = Textures.size();

            for (unsigned int i = 0; i < Textures.size(); i++) {
                if (Textures[i] == Texture) {
                    TextureIndex = i;
                    break;
                }
            }

            if (TextureIndex == (int)Textures.size()) {
                Textures.push_back(std::string(Texture));
                TextureIndex = Textures.size() - 1;
            }

            break;

        case 0x1D:

            StreamIn.seekg(4, std::ios::cur);
            StreamIn.read((char*)&NumTextures, sizeof(int));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);
            StreamIn.read((char*)&AnimTexture[0], sizeof(char) * 32);
            Textures.push_back(std::string(AnimTexture));
            StreamIn.seekg(32 * (NumTextures - 1), std::ios::cur);

            break;

        case 0x0A:

            StreamIn.read((char*)&Color, sizeof(glm::i8vec4));
            Colors.push_back(glm::vec4(1.0f / Color[0], 1.0f / Color[1], 1.0f / Color[2], 1.0f / Color[3]));

            break;

        case 0x19:

            Split.Type = BlockID;
            Split.TextureIndex = TextureIndex;

            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);

            Split.Vertices.resize(NumVertices);

            for (int i = 0; i < NumVertices; i++) {
                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
            }

            if (NumVertices == 3) {

                Faces.push_back(Split);

            } else {

                Triangle.Type = BlockID;
                Triangle.TextureIndex = Colors.size() - 1;

                Triangle.Vertices[0].Index = Split.Vertices[0].Index;
                Triangle.Vertices[1].Index = Split.Vertices[1].Index;
                Triangle.Vertices[2].Index = Split.Vertices[2].Index;
                Faces.push_back(Triangle);

                Triangle.Vertices[0].Index = Split.Vertices[2].Index;
                Triangle.Vertices[1].Index = Split.Vertices[3].Index;
                Triangle.Vertices[2].Index = Split.Vertices[0].Index;
                Faces.push_back(Triangle);

            }

            break;

        case 0x0E:
        case 0x11:
        case 0x18:
        case 0x29:
        case 0x33:
        case 0x34:

            Split.Type = BlockID;
            Split.TextureIndex = TextureIndex;

            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);

            Split.Vertices.resize(NumVertices);

            for (int i = 0; i < NumVertices; i++) {
                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
                StreamIn.read((char*)&X, sizeof(X));
                StreamIn.read((char*)&Y, sizeof(Y));
                Split.Vertices[i].TexCoord = glm::vec2(X / float(65536.0f * 256.0f), Y / float(65536.0f * 256.0f));
//                Split.Vertices[i].TexCoord = glm::vec2(X / float(65280.0f * 256.0f), Y / float(65280.0f * 256.0f));
            }

            if (NumVertices == 3) {

                Faces.push_back(Split);

            } else {

                Triangle.Type = Split.Type;
                Triangle.TextureIndex = Split.TextureIndex;

                Triangle.Vertices[0] = Split.Vertices[0];
                Triangle.Vertices[1] = Split.Vertices[1];
                Triangle.Vertices[2] = Split.Vertices[2];
                Faces.push_back(Triangle);

                Triangle.Vertices[0] = Split.Vertices[2];
                Triangle.Vertices[1] = Split.Vertices[3];
                Triangle.Vertices[2] = Split.Vertices[0];
                Faces.push_back(Triangle);

            }

            break;

        default:

            Logger->Error("Unknown Model Face Type - " + std::to_string(BlockID) + ", Skipping...", true, true, false);

            break;

        }

        StreamIn.read((char*)&BlockID, sizeof(BlockID));

    }

    StreamIn.close();

    BoundingSphere = CalculateBounds(Min, Max);

    CalculateNormals();

    if (TexturePool.Create(PodPool, Textures)) {
        Logger->Text("Create Model Texture Pool");
    } else {
        Logger->Error("Error Creating Model Texture Pool");
    }

    Build();

    return true;

}

void CModel::Destroy(void)
{
    Vertices.clear();
    Normals.clear();
    Textures.clear();
    AnimatedTextures.clear();
    Colors.clear();
    Faces.clear();

    for (unsigned int i = 0; i < Chunks.size(); i++) {

        glDeleteVertexArrays(1, &Chunks[i].VAO);
        Chunks[i].Data.Destroy();
        Chunks[i].DataIndices.Destroy();

    }

//    if (glIsList(GLModel)) {
//        glDeleteLists(GLModel, 1);
//        Logger->Text("Released Model OpenGL Display List Memory - " + Filename);
//    }
}

void CModel::Render(void)
{
    for (unsigned int i = 0; i < Chunks.size(); i++) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Chunks[i].Texture);

        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_CULL_FACE);

        glBindVertexArray(Chunks[i].VAO);
        glDrawElements(GL_TRIANGLES, Chunks[i].Transparent, GL_UNSIGNED_INT, 0);
        glDrawRangeElements(GL_TRIANGLES, 0, Chunks[i].Transparent, Chunks[i].Transparent, GL_UNSIGNED_INT, 0);

        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_CULL_FACE);

        glDrawRangeElements(GL_TRIANGLES, Chunks[i].Transparent, Chunks[i].Indices, Chunks[i].Indices - Chunks[i].Transparent, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

}

bool CModel::Build(void)
{

    Chunks.resize(Textures.size());

    for (unsigned int t = 0; t < Textures.size(); t++) {

        Chunks[t].Indices = 0;
        Chunks[t].Data.Create();
        Chunks[t].DataIndices.Create();

        Chunks[t].Texture = TexturePool.GetTexture(t);

        for (unsigned int f = 0; f < Faces.size(); f++) {

            if ((Faces[f].Type == 0x0E) || (Faces[f].Type == 0x18) /*|| (Faces[f].Type == 0x19)*/ || (Faces[f].Type == 0x29) || (Faces[f].Type == 0x34)) {

                if (Faces[f].TextureIndex == int(t)) {

                    for (int v = 0; v < 3; v++) {

                        Chunks[t].Data.AddData(&Vertices[Faces[f].Vertices[v].Index][0], sizeof(glm::vec3));
                        Chunks[t].Data.AddData(&Faces[f].Vertices[v].TexCoord[0], sizeof(glm::vec2));
                        Chunks[t].Data.AddData(&Normals[Faces[f].Vertices[v].Index][0], sizeof(glm::vec3));
                        Chunks[t].DataIndices.AddData(&Chunks[t].Indices, sizeof(int));
                        Chunks[t].Indices++;

                    }
                }
            }
        }

        Chunks[t].Transparent = Chunks[t].Indices;

        for (unsigned int f = 0; f < Faces.size(); f++) {

            if ((Faces[f].Type == 0x11) || (Faces[f].Type == 0x33)) {

                if (Faces[f].TextureIndex == int(t)) {

                    for (int v = 0; v < 3; v++) {

                        Chunks[t].Data.AddData(&Vertices[Faces[f].Vertices[v].Index][0], sizeof(glm::vec3));
                        Chunks[t].Data.AddData(&Faces[f].Vertices[v].TexCoord[0], sizeof(glm::vec2));
                        Chunks[t].Data.AddData(&Normals[Faces[f].Vertices[v].Index][0], sizeof(glm::vec3));
                        Chunks[t].DataIndices.AddData(&Chunks[t].Indices, sizeof(int));
                        Chunks[t].Indices++;
                    }
                }
            }
        }

        glGenVertexArrays(1, &Chunks[t].VAO);
        glBindVertexArray(Chunks[t].VAO);

        Chunks[t].Data.Bind();
        Chunks[t].Data.DataToGPU(GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

        Chunks[t].DataIndices.Bind(GL_ELEMENT_ARRAY_BUFFER);
        Chunks[t].DataIndices.DataToGPU(GL_STATIC_DRAW);

    }



//    GLModel = glGenLists(1);
//    glNewList(GLModel, GL_COMPILE);
//
//    for (unsigned int t = 0; t < Textures.size(); t++) {
//
//        glDisable(GL_BLEND);
//        glDisable(GL_ALPHA_TEST);
//        glEnable(GL_CULL_FACE);
//
//        TexturePool.Bind(t);
//        glBegin(GL_TRIANGLES);
//
//        for (unsigned int f = 0; f < Faces.size(); f++) {
//
//            if ((Faces[f].Type == 0x0E) || (Faces[f].Type == 0x18) /*|| (Faces[f].Type == 0x19)*/ || (Faces[f].Type == 0x29) || (Faces[f].Type == 0x34)) {
//
//                if (Faces[f].TextureIndex == int(t)) {
//
//                    for (int v = 0; v < 3; v++) {
//
//                        glTexCoord2fv(&Faces[f].Vertices[v].TexCoord[0]);
//                        glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
//                        glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);
//
//                    }
//                }
//            }
//        }
//
//        glEnd();
//
//        glEnable(GL_BLEND);
//        glEnable(GL_ALPHA_TEST);
////        glDisable(GL_CULL_FACE);
//
//        glBegin(GL_TRIANGLES);
//
//        for (unsigned int f = 0; f < Faces.size(); f++) {
//
//            if ((Faces[f].Type == 0x11) || (Faces[f].Type == 0x33)) {
//
//                if (Faces[f].TextureIndex == int(t)) {
//
//                    for (unsigned int v = 0; v < 3; v++) {
//
//                        glTexCoord2fv(&Faces[f].Vertices[v].TexCoord[0]);
//                        glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
//                        glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);
//
//                    }
//                }
//            }
//        }
//
//        glEnd();
//
//    }
//
//    glDisable(GL_BLEND);
//    glDisable(GL_ALPHA_TEST);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glBegin(GL_TRIANGLES);
//
//    for (unsigned int f = 0; f < Faces.size(); f++) {
//
//        if (Faces[f].Type == 0x19 /*== 0x0A*/) {
//
//            for (unsigned int v = 0; v < 3; v++) {
//
//                glColor4fv(&Colors[Faces[f].TextureIndex][0]);
//                glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
//                glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);
//
//            }
//        }
//
//    }
//
//    glEnd();
//
//    glEndList();
//
//    return glIsList(GLModel);
    return true;

}

void CModel::CalculateNormals(void)
{

    glm::vec3 N, A, B;
    std::vector<int> SharedFaces;

    Normals.resize(Vertices.size());
    SharedFaces.resize(Vertices.size());

    for (unsigned int f = 0; f < Faces.size(); f++) {

        A = Vertices[Faces[f].Vertices[2].Index] - Vertices[Faces[f].Vertices[0].Index];
        B = Vertices[Faces[f].Vertices[1].Index] - Vertices[Faces[f].Vertices[0].Index];

        N = glm::cross(B, A);
        N = glm::normalize(N);

        FaceNormals.push_back(N);

        Normals[Faces[f].Vertices[0].Index] += N;
        Normals[Faces[f].Vertices[1].Index] += N;
        Normals[Faces[f].Vertices[2].Index] += N;

        SharedFaces[Faces[f].Vertices[0].Index]++;
        SharedFaces[Faces[f].Vertices[1].Index]++;
        SharedFaces[Faces[f].Vertices[2].Index]++;
    }

    for (unsigned int i = 0; i < Vertices.size(); i++) {
        Normals[i] /= SharedFaces[i];
        Normals[i] = glm::normalize(Normals[i]);
    }

}

float CModel::CalculateBounds(glm::vec3 &Min, glm::vec3 &Max)
{
    BoundingSphere = 0.0f;
    Min = Max = glm::vec3(0);

    for (unsigned int v = 0; v < Vertices.size(); v++) {

        if (Vertices[v].x < Min.x) {
            Min.x = Vertices[v].x;
        }

        if (Vertices[v].y < Min.y) {
            Min.y = Vertices[v].y;
        }

        if (Vertices[v].z < Min.z) {
            Min.z = Vertices[v].z;
        }

        if (Vertices[v].x > Max.x) {
            Max.x = Vertices[v].x;
        }

        if (Vertices[v].y > Max.y) {
            Max.y = Vertices[v].y;
        }

        if (Vertices[v].z > Max.z) {
            Max.z = Vertices[v].z;
        }

    }

    glm::vec3 Dimensions = (Max - Min);
    BoundingSphere = float(Dimensions.x + Dimensions.y + Dimensions.z) * 1.5f;

    return BoundingSphere;

}

glm::vec3 CModel::GetBounds(glm::vec3 &Min, glm::vec3 &Max)
{
    Min = this->Min;
    Max = this->Max;

    return Max - Min;
}


CModelPool::CModelPool(void)
{

}

CModelPool::~CModelPool(void)
{
    Models.clear();
}

bool CModelPool::Create(CPodPool &PodPool, std::vector<std::string> &Filenames)
{
    Models.resize(Filenames.size());

    for (unsigned int i = 0; i < Models.size(); i++) {

        if (Models[i].Load(PodPool, Filenames[i])) {
            Logger->Text("Loaded Bin Model - " + Filenames[i]);
        } else {
            Logger->Error("Error Loading Bin Model - " + Filenames[i]);
        }

    }

    return true;

}
