
#ifndef CAMERA_HPP
#define CAMERA_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <time.h>
#include <string>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

#include "../Physics/Vehicle.hpp"

extern GLFWwindow *GLWindow;

namespace Engine
{

class CCamera
{
public:

    glm::vec3 Centrum;

    glm::vec3 Position;
    glm::vec3 Center;
    glm::vec3 Up;

    float Yaw;
    float Pitch;
    float Angle;
    float Speed;

    unsigned int Boundaries;

    double XPos, YPos;
    double LastX, LastY;
    double XOffset, YOffset;
    unsigned int HalfWidth, HalfHeight;

public:

    CCamera(void);
    ~CCamera(void);

    bool Create(unsigned int Boundaries, const glm::vec3 Position);

    glm::mat4 Update(const double Delta, Physics::CVehicle *Vehicle);

    glm::mat4 LookAt(void) { return glm::lookAt(Position, Center, Up); }

    glm::vec3 GetPosition(void) { return Position; }
    glm::vec3 GetCenter(void) { return Position + Center; }
    glm::vec3 GetUp(void) { return Up; }

};
}

#endif
