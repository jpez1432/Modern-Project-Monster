
#include "Common.hpp"

#ifdef _DEBUG32
const char *Build = "[x86 Debug]";
#elif _RELEASE32
const char *Build = "[x86 Release]";
#elif _RELEASE64
const char *Build = "[x64 Release]";
#endif

std::string GetInfo()
{

    TCHAR szVersionFile[MAX_PATH];
    GetModuleFileName(NULL, szVersionFile, MAX_PATH);

    DWORD verHandle = 0;
    UINT size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD verSize = GetFileVersionInfoSize(szVersionFile, &verHandle);
    char Buffer[64];

    if (verSize != 0) {
        LPSTR verData = new char[verSize];

        if (GetFileVersionInfo(szVersionFile, verHandle, verSize, verData)) {
            if (VerQueryValue(verData, "\\", (VOID FAR * FAR*)&lpBuffer, &size)) {
                if (size) {
                    VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;

                    if (verInfo->dwSignature == 0xfeef04bd) {
                        sprintf(Buffer, "Project Monster - v%d.%d.%d %s",
                                int(verInfo->dwFileVersionMS >> 16) & 0xffff,
                                int(verInfo->dwFileVersionMS >>  0) & 0xffff,
                                int(verInfo->dwFileVersionLS >> 16) & 0xffff,
                                Build
                               );
                    }
                }
            }
        }

        delete[] verData;
    }

    return std::string(Buffer);
}

std::vector<std::string> ListFiles(std::string Folder, std::string Extensions)
{
    std::vector<std::string> Filenames;
    std::string Path = (Folder + "/*.*");// + Extensions);


    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(Path.c_str(), &fd);

    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                Filenames.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }

    return Filenames;
}

std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{

    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);

    } else {
        return "";
    }
}

std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{
    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);

    } else {
        return "";
    }
}

bool StrCompare(const std::string& Str1, const std::string& Str2) {
    return ((Str1.size() == Str2.size()) && equal(Str1.begin(), Str1.end(), Str2.begin(), CaseInsCharCompareN));
}

//bool StrCompare(const std::string& Str1, const std::string& Str2)
//{
//    if (Str1.size() != Str2.size()) return false;
//
//    for (unsigned int i = 0; i < Str2.size(); i++) {
//        if (tolower(Str1[i]) != tolower(Str2[i])) {
//            return false;
//        }
//    }
//    return true;
//}

void ParseVector(std::string Line, glm::vec3 &Vector)
{

    Vector.x = atof(Line.substr(Line.find_last_of(",") + 1, Line.length()).c_str());
    Vector.y = atof(Line.substr(Line.find_first_of(",") + 1, Line.find_last_of(",") - 1 - Line.find_first_of(",") + 1).c_str());
    Vector.z = atof(Line.substr(0, Line.find_first_of(",")).c_str());

}

void Cylinder(glm::vec3 Start, glm::vec3 End, float Radius, int Subdivisions, glm::vec4 Color, bool Lighted)
{

    glDisable(GL_LIGHTING);
    glColor4f(Color.r, Color.g, Color.b, Color.a);

    glm::vec3 Vector = End - Start;


    float V = glm::length(End - Start);
    float Ax = 57.2957795 * glm::acos(Vector.z / V);

    if (Vector.z < 0.0) {
        Ax = -Ax;
    }

    float RotateX = -Vector.y * Vector.z;
    float RotateY = Vector.x * Vector.z;

    float X = 0.0f;
    float Y = 0.0f;

    glPushMatrix();

    glTranslatef(Start.x, Start.y, Start.z);
    glRotatef(Ax, RotateX, RotateY, 0.0f);

    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= Subdivisions; i++) {

        double U = i / (double)Subdivisions;

        X = Radius * cos(2 * 3.1415926f * U);
        Y = Radius * sin(2 * 3.1415926f * U);

        glTexCoord2d(U, 1.0f);
        glVertex3f(X, Y, V);
        glTexCoord2d(U, 0.0f);
        glVertex3f(X, Y, 0);

    }

    glEnd();

    glPopMatrix();

    if (Lighted) {
        glEnable(GL_LIGHTING);
    }

}


void ODEtoOGL(float *M, const dReal *P, const dReal *R)
{
    M[0]  = R[0];
    M[1]  = R[4];
    M[2]  = R[8];
    M[3]  = 0.0f;
    M[4]  = R[1];
    M[5]  = R[5];
    M[6]  = R[9];
    M[7]  = 0.0f;
    M[8]  = R[2];
    M[9]  = R[6];
    M[10] = R[10];
    M[11] = 0.0f;
    M[12] = P[0];
    M[13] = P[1];
    M[14] = P[2];
    M[15] = 1.0f;
}

void OGLtoODE(const dReal* M, const dReal *P, float* R)
{
    R[0]  = M[0];
    R[4]  = M[1];
    R[8]  = M[2];
    R[3]  = 0.0f;
    R[1]  = M[4];
    R[5]  = M[5];
    R[9]  = M[6];
    R[7]  = 0.0f;
    R[2]  = M[8];
    R[6]  = M[9];
    R[10] = M[10];
    R[11] = 0.0f;
    R[12] = P[0];
    R[13] = P[1];
    R[14] = P[2];
    R[15] = 1.0f;
}


