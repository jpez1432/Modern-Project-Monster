
#ifndef GENERIC_HPP
#define GENERIC_HPP

#define dDOUBLE

#include <Ode/Ode.h>

#include "Client.hpp"

namespace Physics
{

class CGeneric : public CClient
{

private:

    dMass M;
    int Type;
    dBodyID Body;
    dGeomID Geom;

public:

    CGeneric(void);
    virtual ~CGeneric(void);

    virtual bool Update(dReal Step);
    virtual void Render(void);

    bool Create(int Type, dReal Width = 0.0, dReal Height = 0.0, dReal Length = 0.0, dReal Radius = 0.0, dReal Mass = 1.0, bool Static = false);

    void SetMass(dReal Mass);
    void SetData(GeomData *Data);
    void SetEnviroment(void);
    void SetPosition(dReal X, dReal Y, dReal Z);
    void SetRotation(dReal X, dReal Y, dReal Z);

};
}

#endif
