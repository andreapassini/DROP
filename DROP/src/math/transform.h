#pragma once

#include <assert.h>
#include "mat3.h"
#include "quaternion.h"
#include "point3.h"
#include "vector3.h"
#include "versor3.h"
#include "scalar.h"

namespace VgMath{

#define ANISOTROPIC_SCALING 0


/* a class for a transformation */
class Transform{
public:
    //glm::vec3 translate;
    Vector3 m_Translate;
#if ANISOTROPIC_SCALING
    Vector3 scale; // (NON-UNIFORM or ANISOTROPIC) scaling
#else
    Scalar m_Scale; // (UNIFORM or ISOTROPIC) scaling
#endif
    Quaternion m_Rotate;

    // empty constructor: returns identity transform
#if ANISOTROPIC_SCALING
    Transform(): translate(0,0,0), scale(1,1,1),  rotate() {}
#else
    Transform(): m_Translate(0,0,0), m_Scale(1),  m_Rotate(Quaternion::angleAxis(Degrees(0.0), Vector3(0.0, 1.0, 0.0).normalized())) {}
    //Transform(): translate(0,0,0), scale(1),  rotate() {}
#endif

    Vector3 operator() (const Vector3& v) const {
        return m_Rotate.apply(m_Scale * v);
    }
    Versor3 operator() (const Versor3& d) const {
        return m_Rotate.apply( d );
    }
    Point3 operator() (const Point3& p) const {
        return m_Rotate.apply(p.scaled(m_Scale)) + m_Translate;
    }
    Scalar operator() (const Scalar& d) const {
        return d*m_Scale;
    }

    // operatoins between transforms
    Transform inverse() const {
        Transform res;
#if ANISOTROPIC_SCALING
        res.scale = Vector3( 1/scale.x, 1/scale.y, 1/scale.z ) ;
#else
        res.m_Scale = 1.0 /m_Scale;
#endif
        res.m_Rotate = m_Rotate.inverse();
        res.m_Translate = res.m_Rotate.apply( -m_Translate * res.m_Scale );
        return res;
    }

    // transform COMPOSITION
    Transform operator * (const Transform & b) const; // composition
};



// the result is the transformation that does b first, then me (this)
inline Transform Transform::operator * (const Transform & b) const 
{
    Transform c;
    c.m_Scale = m_Scale * b.m_Scale;
    c.m_Rotate = m_Rotate * b.m_Rotate;
    c.m_Translate = m_Translate + c.m_Rotate.apply( b.m_Translate * m_Scale);

    return c;
}


inline bool areEqual(const Transform &a , const Transform &b )
{
    return areEqual( a.m_Scale ,    b.m_Scale )
        && areEqual( a.m_Translate, b.m_Translate )
        && areEquivalent( a.m_Rotate,    b.m_Rotate );
}


}; // end of namespace definition
