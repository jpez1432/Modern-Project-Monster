
#ifndef COMMON_HPP
#define COMMON_HPP

#define dDOUBLE
#define GLEW_STATIC

#include <windows.h>
#include <string>
#include <vector>
#include <cstdio>
#include <GL/Glew.h>
#include <Ode/Ode.h>
#include <glm.hpp>

std::string GetInfo();
std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);

inline bool CaseInsCharCompareN(char A, char B) { return(std::toupper(A) == std::toupper(B)); }
bool StrCompare(const std::string& Str1, const std::string& Str2);

void ODEtoOGL(float *M, const dReal *P, const dReal *R);
void OGLtoODE(const dReal* M, const dReal *P, float* R);

void Cylinder(glm::vec3 Start, glm::vec3 End, float Radius, int Subdivisions, glm::vec4 Color, bool Lighted = true);

void ParseVector(std::string Line, glm::vec3 &Vector);

std::vector<std::string> ListFiles(std::string Folder, std::string Extensions);

#endif

