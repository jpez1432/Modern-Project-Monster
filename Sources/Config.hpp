
#ifndef CCONFIG_HPP
#define CCONFIG_HPP

#include <Glm.hpp>

#include "Logger.hpp"

extern CLogger *Logger;
extern std::string ConfigFile;

class CConfig
{

private:

public:

    struct {
        bool Paused;
        bool HelpMode;
        bool EditMode;
        bool DebugMode;
        bool PhysicsMode;
        bool FpsCounter;
    } General;

    struct {
        char WorldFile[32];
        char VehicleFile[32];
    } Physics;

    struct {
        bool Pods;
        bool Files;
        char PodIni[32];
        char Track[32];
        char Vehicle[32];
        int Weather;
        int Scenery;
    } Game;

    struct {
        char Name[32];
        bool Bold;
        unsigned int Invert;
        unsigned int Size;
    } Font;

    struct {
        bool Wireframe;
        bool Textured;
        bool Lighted;
        bool Culled;
        bool Reflections;
        bool Terrain;
        bool Boxes;
        bool Models;
        bool Water;
        bool Backdrop;
        bool Fog;
        bool Wrap;
    } Render;

    struct {
        int Width;
        int Height;
        int Monitor;
        int RefreshRate;
        int Antialiasing;
        float FieldOfVision;
        float AspectRatio;
        float NearClip;
        float FarClip;
        bool VSync;
        bool DoubleBuffer;
        bool Fullscreen;
        bool Multisample;
        int TextureQuality;
    } Graphics;

    struct {
        bool Invert;
        unsigned int Mode;
        glm::vec3 Position;
        glm::vec3 Center;
        glm::vec3 Up;
        float Yaw;
        float Pitch;
        float Radius;
        float Angle;
    } Camera;

    struct {
        bool Joystick;
        bool MouseLook;
        float MouseSensitivity;
        float MovementSpeed;
        unsigned int Gas;
        unsigned int Brake;
        unsigned int ShiftUp;
        unsigned int ShiftDown;
        unsigned int UpShift;
        unsigned int DownShift;
        unsigned int FrontSteerLeft;
        unsigned int FrontSteerRight;
        unsigned int RearSteerLeft;
        unsigned int RearSteerRight;
        unsigned int Forwards;
        unsigned int Backwards;
        unsigned int StrafeLeft;
        unsigned int StrafeRight;
        unsigned int Up;
        unsigned int Down;
    } Input;

    struct {
        int Flip;
        int Reset;
        int Gas;
        int Brake;
        int UpShift;
        int DownShift;
        int FrontSteerLeft;
        int FrontSteerRight;
        int RearSteerLeft;
        int RearSteerRight;
    } Joystick;

    void Defaults(std::string Filename = "");

public:

    CConfig(void);
    CConfig(std::string Filename);
    ~CConfig(void);

    bool Load(std::string Filename);
    bool Save(std::string Filename = "");

};

#endif
