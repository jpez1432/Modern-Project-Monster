
#ifndef TIMER_HPP
#define TIMER_HPP

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

namespace Engine
{

class CTimer
{

private:

    double LastTime;
    double CurrentTime;
    double DeltaTime;
    double LastFrame;

    unsigned int Counter;
    unsigned int Fps;

public:

    CTimer(void);
    ~CTimer(void);

    const double Update(void);
    void Reset(void);

    unsigned int GetFps(void)
    {
        return Fps;
    }

    const double GetDelta(void)
    {
        return DeltaTime;
    }

};

}

#endif
