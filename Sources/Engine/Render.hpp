
#ifndef RENDER_HPP
#define RENDER_HPP

#define GLEW_STATIC

#include <GL/Glew.h>

#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>

#include "../Interface/imgui.h"
#include "../Interface/imgui_impl_glfw.h"
#include "../Interface/imgui_impl_opengl3.h"

#include "Input.hpp"
#include "QuadTree.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"
#include "Shaders.hpp"
#include "../Config.hpp"
#include "../Game/Track.hpp"
#include "../Game/Trucks.hpp"
#include "../Physics/Server.hpp"
#include "../Physics/Client.hpp"
#include "../Physics/Generic.hpp"
#include "../Physics/Vehicle.hpp"

extern unsigned int Fps;

extern Engine::CInput *Input;
extern std::string InterfaceFile;

namespace Engine
{

class CRender
{

private:

    GLuint LogoVAO;
    Engine::CVBO LogoData;

    GLuint WaterVAO;
    Engine::CVBO WaterData;

    glm::mat4 ProjectionMatrix;
    glm::mat4 OrthoMatrix;

    GLuint PerspectiveView;
    GLuint OrthographicalView;

    Physics::CVehicle *Vehicle;

    Engine::CQuadTree *QuadTree;
    Engine::CCamera Camera;
    Engine::CInput Input;
    Engine::CFrustum Frustum;

    Engine::CShader FragShader;
    Engine::CShader VertShader;
    Engine::CShader FogShader;
    Engine::CShader LightShader;

    Engine::CProgram MainShader;
    Engine::CProgram ColorizeShader;
    Engine::CProgram ProjectionShader;
    Engine::CProgram OrthoShader;

    Game::CTexture LogoTexture;
    Game::CTrack *Track;
    Game::CPodPool *PodPool;
    Game::CWeather Weather;



public:

    CRender(void);
    ~CRender(void);

    bool Create(Game::CPodPool &PodPool, Game::CTrack &Track, Physics::CVehicle &Vehicle);

    void Scene(float Delta);
    void Editor(float Delta);

    void Interface(void);
    void Logo(float Delta);

    void Reflections(void);
    void Backdrop(bool Invert);
    void Models(bool Invert);
    void Water(float Delta);
    void Tree(CNode *Node, int XOffset, int ZOffset, bool Invert);

    void Resize(void);

};

}

#endif
