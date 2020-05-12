
#include "Weather.hpp"

using namespace Game;

CWeather::CWeather(void)
{

}

CWeather::~CWeather(void)
{

}

void CWeather::Create(CPodPool &PodPool, unsigned int Weather, Engine::CProgram &Shader)
{
    this->Shader = &Shader;

    switch (Weather) {

    case 0:	// Clear SkyDome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 1:	// Cloudy SkyDome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.8f;
        GeneralTint[2] = 0.8f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 2:	// Fog Skydome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 3:	// Dense Fog Skydome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 4:	// Rainy SkyDome
        SkyDomeModel = "CCLOUDS.BIN";
        GeneralTint[0] = 0.5f;
        GeneralTint[1] = 0.5f;
        GeneralTint[2] = 0.5f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 5:	// Dusk SkyDome
        SkyDomeModel = "DUSKSKY.BIN";
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.7f;
        GeneralTint[2] = 0.6f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 6:	// Night SkyDome
        SkyDomeModel = "NITESKY.BIN";
        GeneralTint[0] = 0.3f;
        GeneralTint[1] = 0.3f;
        GeneralTint[2] = 0.3f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 7:	// Night SkyDome (Pitch Black)
        SkyDomeModel = "NITESKY.BIN";
        GeneralTint[0] = 0.1f;
        GeneralTint[1] = 0.1f;
        GeneralTint[2] = 0.1f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;
    }

    WaterTint[0] = 0.6f;
    WaterTint[1] = 0.8f;
    WaterTint[2] = 1.0f;
    WaterTint[3] = 0.4f;

    GeneralTint[3] = BackdropTint[3] = 1.0f;

    SkyDome.Destroy();

    if (SkyDome.Load(PodPool, SkyDomeModel)) {
        Logger->Text("Loaded Weather SkyDome Model - " + SkyDomeModel);
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - " + SkyDomeModel);
    }
}

void CWeather::ApplyTint(void)
{
    Shader->Bind();
//    Shader->SetUniform("LightColor", GeneralTint);
    Shader->SetUniform("sunLight.vColor", GeneralTint);
    Shader->SetUniform("sunLight.vAmbient", 0.75f);


    if (Config->Render.Lighted) {
        Shader->SetUniform("bLighted", 1);
    } else {
        Shader->SetUniform("bLighted", 0);
    }
}

void CWeather::ApplyBackdropTint(void)
{
    Shader->Bind();
    Shader->SetUniform("bLighted", 1);
    Shader->SetUniform("LightColor", BackdropTint);
}

void CWeather::ApplyWaterTint(void)
{
    Shader->Bind();
    Shader->SetUniform("bLighted", 1);
    Shader->SetUniform("LightColor", WaterTint);
}
