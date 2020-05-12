
#include "Config.hpp"

CConfig::CConfig(void)
{
}

CConfig::CConfig(std::string Filename)
{
    Load(Filename);
}

CConfig::~CConfig(void)
{
}

void CConfig::Defaults(std::string Filename)
{

    if (Filename == "") {
        Filename = ConfigFile;
    }

    General.Paused = true;
    General.HelpMode = false;
    General.EditMode = true;
    General.DebugMode = true;
    General.PhysicsMode = true;
    General.FpsCounter = true;

    strcpy(Physics.WorldFile, "Default.world");
    strcpy(Physics.VehicleFile, "Default.vehicle");

    Game.Pods = true;
    Game.Files = true;
    strcpy(Game.PodIni, "Pod.ini");
    strcpy(Game.Track, "Crazy98.sit");
    strcpy(Game.Vehicle, "Bigfoot.trk");
    Game.Weather = 0;
    Game.Scenery = 3;

    strcpy(Font.Name, "Arialbd.ttf");
    Font.Bold = true;
    Font.Invert = true;
    Font.Size = 16;

    Render.Wireframe = false;
    Render.Textured = true;
    Render.Lighted = true;
    Render.Culled = true;
    Render.Reflections = true;
    Render.Terrain = true;
    Render.Boxes = true;
    Render.Models = true;
    Render.Water = true;
    Render.Backdrop = true;
    Render.Fog = true;
    Render.Wrap = false;

    Graphics.Width = 900;
    Graphics.Height = 600;
    Graphics.Monitor = 0;
    Graphics.RefreshRate = 60;
    Graphics.Antialiasing = 2;
    Graphics.FieldOfVision = 60.0f;
    Graphics.AspectRatio = 1.5f;
    Graphics.NearClip = 0.01f;
    Graphics.FarClip = 64.0f;
    Graphics.VSync = true;
    Graphics.DoubleBuffer = true;
    Graphics.Fullscreen = false;
    Graphics.Multisample = true;
    Graphics.TextureQuality = 2;

    Camera.Invert = true;
    Camera.Mode = 1;
    Camera.Position = glm::vec3(128.0f, 8.0f, 128.0f);
    Camera.Center = glm::vec3(0.0f, 0.0f, 1.0f);
    Camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Camera.Yaw = 0.0f;
    Camera.Pitch = 0.0f;
    Camera.Radius = 2.0f;

    Input.Joystick = true;
    Input.MouseLook = true;
    Input.MouseSensitivity = 0.1f;
    Input.MovementSpeed = 10.0f;
    Input.Gas = GLFW_KEY_W;
    Input.Brake = GLFW_KEY_S;
    Input.UpShift = GLFW_KEY_E;
    Input.DownShift = GLFW_KEY_Q;
    Input.FrontSteerLeft = GLFW_KEY_A;
    Input.FrontSteerRight = GLFW_KEY_D;
    Input.RearSteerLeft = GLFW_KEY_A;
    Input.RearSteerRight = GLFW_KEY_D;
    Input.Forwards = GLFW_KEY_UP;
    Input.Backwards = GLFW_KEY_DOWN;
    Input.StrafeLeft = GLFW_KEY_LEFT;
    Input.StrafeRight = GLFW_KEY_RIGHT;
    Input.Up = GLFW_KEY_PAGE_UP;
    Input.Down = GLFW_KEY_PAGE_DOWN;

    Joystick.Flip = 2;
    Joystick.Reset = 1;
    Joystick.Gas = 5;
    Joystick.Brake = 4;
    Joystick.UpShift = 7;
    Joystick.DownShift = 6;
    Joystick.FrontSteerLeft = 0;
    Joystick.FrontSteerRight = 0;
    Joystick.RearSteerLeft = -2;
    Joystick.RearSteerRight = -2;



    Logger->Text("Loaded Default Configuration...");

}

bool CConfig::Load(std::string Filename)
{

    Defaults(Filename);
    return true;

    std::ifstream CfgFile(Filename.c_str(), std::ios::in | std::ios::binary);

    if (CfgFile.is_open()) {

        CfgFile.read((char*) &General, sizeof(General));
        CfgFile.read((char*) &Physics, sizeof(Physics));
        CfgFile.read((char*) &Game, sizeof(Game));
        CfgFile.read((char*) &Font, sizeof(Font));
        CfgFile.read((char*) &Render, sizeof(Render));
        CfgFile.read((char*) &Graphics, sizeof(Graphics));
        CfgFile.read((char*) &Input, sizeof(Input));
        CfgFile.read((char*) &Camera, sizeof(Camera));
        CfgFile.read((char*) &Joystick, sizeof(Joystick));

        CfgFile.close();
        Logger->Text("Loaded Configuration");

    } else {
        Logger->Error("Error Loading Configuration, Generating Defaults...", true, true, false);
        Defaults(Filename);
    }


    return true;
}

bool CConfig::Save(std::string Filename)
{

    std::ofstream CfgFile(Filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if (CfgFile.is_open()) {

        CfgFile.write((char*) &General, sizeof(General));
        CfgFile.write((char*) &Physics, sizeof(Physics));
        CfgFile.write((char*) &Game, sizeof(Game));
        CfgFile.write((char*) &Font, sizeof(Font));
        CfgFile.write((char*) &Render, sizeof(Render));
        CfgFile.write((char*) &Graphics, sizeof(Graphics));
        CfgFile.write((char*) &Input, sizeof(Input));
        CfgFile.write((char*) &Camera, sizeof(Camera));
        CfgFile.write((char*) &Joystick, sizeof(Joystick));

        CfgFile.close();
        Logger->Text("Saved Configuration");

    } else {
        Logger->Error("Error Saving Configuration, Aborting...");
    }

    return true;
}

