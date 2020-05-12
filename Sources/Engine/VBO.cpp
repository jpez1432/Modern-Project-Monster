
#include "vbo.hpp"

using namespace Engine;

CVBO::CVBO(void) : Size(0), CurrentSize(0), Type(0), GLBuffer(0)
{

}

CVBO::~CVBO(void)
{
    Destroy();
}

bool CVBO::Create(int DefaultSize)
{
    glGenBuffers(1, &GLBuffer);
    Data.reserve(DefaultSize);
    Size = DefaultSize;
    CurrentSize = 0;

    return glIsBuffer(GLBuffer);
}

bool CVBO::Destroy(void)
{
    if (glIsBuffer(GLBuffer)) {
        glDeleteBuffers(1, &GLBuffer);
        Data.clear();
    }

    return !glIsBuffer(GLBuffer);
}

void CVBO::Bind(int BufferType)
{
    Type = BufferType;
    glBindBuffer(Type, GLBuffer);
}

void CVBO::DataToGPU(int DrawingHint)
{
    glBufferData(Type, Data.size(), &Data[0], DrawingHint);
    Data.clear();
}

void CVBO::AddData(void* PtrData, unsigned int DataSize)
{
    Data.insert(Data.end(), (char*)PtrData, (char*)PtrData + DataSize);
    CurrentSize += DataSize;
}

void* CVBO::GetDataPointer(void)
{
    return (void*) &Data[0];
}
