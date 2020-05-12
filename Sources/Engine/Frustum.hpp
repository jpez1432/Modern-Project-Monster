
//#ifndef FRUSTUM_HPP
//#define FRUSTUM_HPP
//
//#include <glm.hpp>
//
//#include "../Config.hpp"
//
//extern CConfig *Config;
//
//namespace Engine
//{
//class CFrustum
//{
//
//private:
//
//    enum Planes {TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP};
//
//    struct Plane {
//        glm::vec3 Normal;
//        glm::vec3 Point;
//        float D;
//    };
//
//    glm::vec3 Ntl, Ntr, Nbl, Nbr;
//    glm::vec3 Ftl, Ftr, Fbl, Fbr;
//    float Nw, Nh, Fw, Fh, Tang;
//
//    Plane Planes[6];
//
//public:
//
//    enum Results {Outside = 0, Intersects, Inside};
//
//    CFrustum(void);
//    ~CFrustum(void);
//
//    void Update(glm::vec3 Position, glm::vec3 Look, glm::vec3 Up);
//    void Resize(void);
//
//    const float Distance(int Index, glm::vec3 Point);
//    void NormalAndPoint(int Index, glm::vec3 Normal, glm::vec3 Point);
//
//    const Results PointInFrustum(glm::vec3 Position);
//    const Results SphereInFrustum(glm::vec3 Position, float Radius);
//    const Results BoxInFrustum(glm::vec3 Min, glm::vec3 Max);
//
//};
//
//}
//
//#endif


#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

namespace Engine
{

class CFrustum
{

private:

    glm::vec4 Planes[6];

    enum PlaneFaces { Right = 0, Left, Bottom, Top, Back, Front };

public:

    CFrustum(void);
    ~CFrustum(void);

    void Update(glm::mat4 Projection, glm::mat4 Model);
    void UpdateFast(glm::mat4 Projection, glm::mat4 Model);

    enum Results { Outside = 0, Intersects, Inside };

    Results BoxInFrustum(glm::vec3 Min, glm::vec3 Max);
    Results SphereInFrustum(glm::vec3 Position, float Size);
    Results PointInFrustum(glm::vec3 Point);
};

}


#endif
