
#ifndef CLIENT_HPP
#define CLIENT_HPP

#define dDOUBLE

#include <Ode/Ode.h>

namespace Physics
{

struct GeomData {
    dReal Slip;
};

class CClient
{

protected:

    dWorldID World;
    dSpaceID Space;
    dSpaceID EnvSpace;
    GeomData EnvData;

public:

    CClient(void);
    virtual ~CClient(void);

    bool Initialize(dWorldID World, dSpaceID Space, dSpaceID EnvSpace);

    virtual bool Update(dReal Step) = 0;
    void Render(void);

};
}

#endif
