
#include "Timer.hpp"

using namespace Engine;

CTimer::CTimer(void)
{
    LastTime = CurrentTime = glfwGetTime();
    DeltaTime = Counter = LastFrame = 0;
}

CTimer::~CTimer(void)
{

}

void CTimer::Reset(void)
{
    LastTime = CurrentTime = glfwGetTime();
    DeltaTime = Counter = LastFrame = 0;
}

const double CTimer::Update(void)
{

    CurrentTime = glfwGetTime();
    Counter++;

    if ((CurrentTime - LastTime) > 1.0f) {
        Fps = Counter;
        Counter = 0;
        LastTime++;
    }

    DeltaTime = CurrentTime - LastFrame;
    LastFrame = CurrentTime;

    return DeltaTime;

}
