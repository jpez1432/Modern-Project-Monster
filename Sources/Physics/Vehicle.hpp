
#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#define dDOUBLE

#include <Ode/Ode.h>

#include <gtc/type_ptr.hpp>

#include "../Game/Models.hpp"
#include "../Game/Trucks.hpp"
#include "../Engine/Shaders.hpp"
#include "../Engine/Input.hpp"

#include "Server.hpp"
#include "Client.hpp"

extern GLFWwindow *GLWindow;

extern Engine::CInput *Input;

namespace Physics
{

class CVehicle : public CClient, Game::CTruck
{

public:

    struct {
        bool Automatic;
        int Gear;
        int GearCount;
        dReal GearAccel[3][3];
        dReal GearNext[3][3];
        dReal Speed;
        dReal Rpms;
        dReal MaxRpms;
        dReal Brake;
        dReal MaxSpeed;
        dReal ReverseSpeed;
        dReal Drag;
        dReal HorsePower;
        dReal Acceleration;
    } Transmission;

    struct Wheel {
        glm::vec3 Position;
        dReal Mass;
        dReal SteerMultiplier;
        dReal DriveMultiplier;
        dReal Damping;
        dReal Error;
        dReal StopERP;
        dReal StopCFM;
        dReal Bounce;
        dReal CFM;
        dReal FMax;
        dReal FudgeFactor;
        dGeomID Geom;
        dBodyID Body;
        dGeomID ShockUpper[2];
        dGeomID ShockLower[2];
        GeomData Data;
    };

    struct Axle {
        glm::vec3 Position;
        dReal Mass;
        dReal MinSteer;
        dReal MaxSteer;
        dReal SteerSpeed;
        dReal SteerAngle;
        dReal CurrentSteer;
        dGeomID Geom;
        dBodyID Body;
        GeomData Data;
        Wheel Wheels[2];
        dJointID Motor[2];
        dJointID Hub[2];
        dJointID FourLink[2];
        dJointID DriveShaft;
    };

    struct {
        dReal MassScale;
        dReal Mass;
        dReal Speed;
        dReal WheelForce;
        dBodyID Body;
        GeomData Data;
        Axle Axles[2];
    } Vehicle;


    dSpaceID VehicleSpace;
    dGeomID CollisionPoints[12];

    float Velocity;
    int Boundaries;

    glm::vec3 Center, Rotation;

    double DownShift(unsigned int Index);
    double UpShift(unsigned int Index);

    double BestGear(unsigned int Index);
    void CheckGear(double Delta);
    void AutomaticGear(void);

    void AddGeom(dGeomID Geom, dBodyID Body, dReal xOffset, dReal yOffset, dReal zOffset, dMatrix3 R);

    void SetOrientation(glm::vec3 Center, glm::vec3 Rotation);
    void SetPosition(glm::vec3 Center);

public:

    CVehicle(void);
    virtual ~CVehicle(void);

    bool LoadPhysics(std::string Filename);
    bool SavePhysics(std::string Filename);

    bool LoadTruck(Game::CPodPool &PodPool, std::string Filename)
    {
        return Load(PodPool, Filename);
    }

    bool Create(glm::vec3 Center, glm::vec3 Rotate, int Boundaries);
    void Destroy(void);

    bool Defaults(std::string Filename = "Default.vehicle");

    void SetPhysics(void);

    float GetVelocity(void);

    int GetGear(void)
    {
        return Transmission.Gear;
    }

    float GetSpeed(void)
    {
        return Transmission.Speed;
    }

    float GetRpms(void)
    {
        return Transmission.Rpms;
    }

    void Reset(void);
    void Flip(void);

    virtual bool Update(dReal Delta);
    void Render(Engine::CProgram &Shader, glm::mat4 LookAt, glm::vec3 LightPos);
    void RenderParts(glm::mat4 LookAt);

    const glm::vec3 GetPosition(void)
    {
        const dReal *Position = dBodyGetPosition(Vehicle.Body);
        return glm::vec3(Position[0], Position[1], Position[2]);
    }

    const glm::vec3 GetWheelPosition(unsigned int Axle, unsigned int Wheel)
    {
        const dReal *Position = dBodyGetPosition(Vehicle.Axles[Axle].Wheels[Wheel].Body);
        return glm::vec3(Position[0], Position[1], Position[2]);
    }



};
}

#endif
