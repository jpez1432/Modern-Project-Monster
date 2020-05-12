
#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "Models.hpp"
#include "../Engine/Shaders.hpp"

namespace Game
{

class CWeather
{

private:

    glm::vec4 GeneralTint;
    glm::vec4 BackdropTint;
    glm::vec4 WaterTint;

    std::string SkyDomeModel;
    Game::CModel SkyDome;

    Engine::CProgram *Shader;

public:

    CWeather(void);
    ~CWeather(void);

    void Create(CPodPool &PodPool, unsigned int Weather, Engine::CProgram &Shader);

    void ApplyTint(void);
    void ApplyBackdropTint(void);
    void ApplyWaterTint(void);

    void DrawSkyDome(void)
    {
        SkyDome.Render();
    }
    glm::vec4 &GetBackdropTint(void) { return BackdropTint; }
    glm::vec4 &GetGeneralTint(void) { return GeneralTint; }
    glm::vec4 &GetWaterTint(void) { return WaterTint; }

};

}

#endif
