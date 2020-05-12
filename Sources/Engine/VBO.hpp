
#ifndef VBO_HPP
#define VBO_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <vector>

namespace Engine
{

class CVBO
{

private:

    int Size;
    int CurrentSize;
    int Type;
    unsigned int GLBuffer;

    std::vector<unsigned char>Data;

public:

    CVBO(void);
    ~CVBO(void);

    bool Create(int DefaultSize = 0);
    bool Destroy(void);

    void AddData(void* PtrData, unsigned int DataSize);
    void DataToGPU(int DrawingHint);

    void Bind(int BufferType = GL_ARRAY_BUFFER);

    void* GetDataPointer(void);

    unsigned int BufferID(void)
    {
        return GLBuffer;
    }
    unsigned int GetSize(void)
    {
        return CurrentSize;
    }

};
}

#endif
