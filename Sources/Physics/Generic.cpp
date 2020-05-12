
#include "Generic.hpp"

using namespace Physics;

CGeneric::CGeneric(void) : Body(0), Geom(0)
{

    EnvData.Slip = 0.000001f;

}

CGeneric::~CGeneric(void)
{

    if (Body) {
        dBodyDestroy(Body);
        Body = 0;
    }

    if (Geom) {
        dGeomDestroy(Geom);
        Geom = 0;
    }

}

bool CGeneric::Update(dReal Step)
{

    if (Body == 0 || Geom == 0) {
        return false;
    }

    Render();

    return true;

}

void CGeneric::Render(void)
{

//    dVector3 Sides;
////    dReal Radius;
//    dReal Width, Height, Length;
//
//    float Matrix[16];
//
////	GLUquadric *Object = gluNewQuadric();
//
//    const dReal *Position = dGeomGetPosition(Geom);
//    const dReal *Rotation = dGeomGetRotation(Geom);
//
//    ODEtoOGL(Matrix, Position, Rotation);
//
//    glPushMatrix();
//
//    glMultMatrixf(Matrix);
//
//    switch (Type) {
//
//    case dSphereClass:
//
////                Radius = dGeomSphereGetRadius(Geom);
////                gluSphere(Object, Radius, 10, 10);
//
//        break;
//
//    case dBoxClass:
//
//        dGeomBoxGetLengths(Geom, Sides);
//
//        Width = Sides[0] * 0.5f;
//        Height = Sides[1] * 0.5f;
//        Length = Sides[2] * 0.5f;
//
////                glBegin(GL_TRIANGLE_STRIP);
////
////                    glNormal3f (-1.0f, 0.0f, 0.0f);
////                    glVertex3f (-Width, -Height, -Length);
////                    glVertex3f (-Width, -Height, Length);
////                    glVertex3f (-Width, Height, -Length);
////                    glVertex3f (-Width, Height, Length);
////
////                    glNormal3f (0.0f, 1.0f, 0.0f);
////                    glVertex3f (Width, Height, -Length);
////                    glVertex3f (Width, Height, Length);
////
////                    glNormal3f (1.0f, 0.0f, 0.0f);
////                    glVertex3f (Width, -Height, -Length);
////                    glVertex3f (Width, -Height, Length);
////
////                    glNormal3f (0.0f, -1.0f, 0.0f);
////                    glVertex3f (-Width, -Height, -Length);
////                    glVertex3f (-Width, -Height, Length);
////
////                glEnd();
////
////                glBegin(GL_TRIANGLE_FAN);
////
////                    glNormal3f (0.0f, 0.0f, 1.0f);
////                    glVertex3f (-Width, -Height, Length);
////                    glVertex3f (Width, -Height, Length);
////                    glVertex3f (Width, Height, Length);
////                    glVertex3f (-Width, Height, Length);
////
////                glEnd();
////
////                glBegin(GL_TRIANGLE_FAN);
////
////                    glNormal3f (0.0f, 0.0f, -1.0f);
////                    glVertex3f (-Width, -Height, -Length);
////                    glVertex3f (-Width, Height, -Length);
////                    glVertex3f (Width, Height, -Length);
////                    glVertex3f (Width, -Height, -Length);
////
////                glEnd();
//
//        break;
//
//    case dCCylinderClass:
//
////                dGeomCCylinderGetParams(Geom, &Radius, &Length);
////                gluCylinder(Object, Radius, Radius, Length, 10, 10);
////
////                glPushMatrix();
////                    glTranslatef(0, 0, -Length / 2 + Radius);
////                    gluSphere(Object, Radius, 10, 10);
////                glPopMatrix();
////
////                glPushMatrix();
////                    glTranslatef(0, 0, Length / 2 + Radius);
////                    gluSphere(Object, Radius, 10, 10);
////                glPopMatrix();
//
//        break;
//
//    case dCylinderClass:
//
////                dGeomCylinderGetParams(Geom, &Radius, &Length);
////                gluCylinder(Object, Radius, Radius, Length, 10, 10);
////
////                glPushMatrix();
////                    glTranslatef(0, 0, -Length / 2 + Radius);
////                    gluDisk(Object, 0, Radius, 10, 10);
////                glPopMatrix();
////
////                glPushMatrix();
////                    glTranslatef(0, 0, Length / 2 + Radius);
////                    gluDisk(Object, 0, Radius, 10, 10);
////                glPopMatrix();
////
////                break;
//
//    default:
//
//        return;
//
//    }
//
//    glPopMatrix();
//
////    gluDeleteQuadric(Object);

}

bool CGeneric::Create(int Type, dReal Width, dReal Height, dReal Length, dReal Radius, dReal Mass, bool Static)
{

    if (World == 0 || Space == 0) {
        return false;
    }

    this->Type = Type;

    switch (Type) {

    case dSphereClass:

        Geom = dCreateSphere(0, Radius);
        SetData(&EnvData);

        if (!Static) {
            Body = dBodyCreate(World);
            dMassSetSphere(&M, 1, Radius);
            dBodySetMass(Body, &M);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dBoxClass:

        Geom = dCreateBox(0, Width, Height, Length);
        SetData(&EnvData);

        if (!Static) {
            Body = dBodyCreate(World);
            dMassSetBox(&M, 1, Width, Height, Length);
            dBodySetMass(Body, &M);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dCCylinderClass:

        Geom = dCreateCCylinder(0, Radius, Length);
        SetData(&EnvData);

        if (!Static) {
            Body = dBodyCreate(World);
            dMassSetCappedCylinder(&M, 1, 1, Radius, Length);
            dBodySetMass(Body, &M);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dCylinderClass:

        Geom = dCreateCylinder(0, Radius, Length);
        SetData(&EnvData);

        if (!Static) {
            Body = dBodyCreate(World);
            dMassSetCylinder(&M, 1, 1, Radius, Length);
            dBodySetMass(Body, &M);
            dGeomSetBody(Geom, Body);
        }

        break;

    }

    return true;

}

void CGeneric::SetMass(dReal Mass)
{

    if (Body == 0 || Geom == 0) {
        return;
    }

    dBodyGetMass(Body, &M);
    dMassAdjust(&M, Mass);
    dBodySetMass(Body, &M);

}

void CGeneric::SetData(GeomData *Data)
{
    memcpy(&EnvData, Data, sizeof(GeomData));

}

void CGeneric::SetEnviroment(void)
{
    dSpaceAdd(Space, Geom);
}

void CGeneric::SetPosition(dReal X, dReal Y, dReal Z)
{
    dGeomSetPosition(Geom, X, Y, Z);
}

void CGeneric::SetRotation(dReal X, dReal Y, dReal Z)
{

//    const dReal *Position = dGeomGetPosition(Geom);
//    dMatrix3 Rotation;
//    dRSetIdentity(Rotation);
//    dRFromEulerAngles(Rotation, X, Y, Z);
//    dGeomSetRotation(Geom, Rotation);

}


