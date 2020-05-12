
#ifndef SERVER_HPP
#define SERVER_HPP

#define dDOUBLE

#include <Ode/Ode.h>

#include <list>

#include "../Game/Terrain.hpp"
#include "Client.hpp"

namespace Physics
{

class CServer
{

public:

    struct {
        dReal MassScale;
        dReal Gravity;
        dReal ERP;
        dReal CFM;
        dReal Slip;
        bool QuickStep;
        bool VariableStep;
        dReal StepSize;
        int NumIterations;
    } World;

    struct {
        bool AutoDisableFlag;
        dReal AutoDisableLinearThreshold;
        dReal AutoDisableAngularThreshold;
        int AutoDisableSteps;
        dReal AutoDisableTime;
        dReal Bounce;
        dReal BounceVelocity;
        dReal SoftERP;
        dReal SoftCFM;
    } Collisions;

    struct {
        dReal LinearDamping;
        dReal AngularDamping;
        dReal LinearDampingThreshold;
        dReal AngularDampingThreshold;
    } Damping;

    struct {
        int NumContacts;
        dReal MaxCorrectingVelocity;
        dReal SurfaceLayer;
    } Contacts;

public:

    dWorldID ODEWorld;
    dSpaceID Space;
    dSpaceID EnvSpace;
    dJointGroupID ContactGroup;
    dGeomID HeightField;
    dHeightfieldDataID HeightID;
    dGeomID Plane;
    GeomData EnvData;

    Game::CTerrain *Terrain;
    std::list<CClient*> ClientList;
    static CServer Instance;

    void CheckCollisions(void *data, dGeomID o1, dGeomID o2);
    static void CollisionCallback(void *data, dGeomID o1, dGeomID o2);

public:

    CServer(void);
    virtual ~CServer(void);

    bool LoadPhysics(std::string Filename);
    bool SavePhysics(std::string Filename);

    bool Create(Game::CTerrain *Terrain);
    bool SetTerrain(Game::CTerrain *Terrain);
    bool Destroy(void);

    bool Defaults(std::string Filename = "Default.world");

    void SetPhysics(void);

    bool Update(dReal StepSize);

    bool RegisterClient(CClient *Client);
    bool UnregisterClient(CClient *Client);

    static CServer *GetInstance()
    {
        return &Instance;
    }


};
}


#endif
