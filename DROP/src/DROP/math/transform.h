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
    Vector3 translate;
#if ANISOTROPIC_SCALING
    Vector3 scale; // (NON-UNIFORM or ANISOTROPIC) scaling
#else
    Scalar scale; // (UNIFORM or ISOTROPIC) scaling
#endif
    Quaternion rotate;

public:
    // empty constructor: returns identity transform
#if ANISOTROPIC_SCALING
    Transform(): translate(0,0,0), scale(1,1,1),  rotate() {}
#else
    Transform(): translate(0,0,0), scale(1),  rotate(Quaternion::angleAxis(Degrees(0.0), Vector3(0.0, 1.0, 0.0).normalized())) {}
    //Transform(): translate(0,0,0), scale(1),  rotate() {}
#endif

    Vector3 operator() (const Vector3& v) const {
        return rotate.apply(scale * v);
    }
    Versor3 operator() (const Versor3& d) const {
        return rotate.apply( d );
    }
    Point3 operator() (const Point3& p) const {
        return rotate.apply(p.scaled(scale)) + translate;
    }
    Scalar operator() (const Scalar& d) const {
        return d*scale;
    }

    Versor3 Up() const;

    void Up(Versor3& outUp) const;


    // operations between transforms
    Transform inverse() const {
        Transform res;
#if ANISOTROPIC_SCALING
        res.scale = Vector3( 1/scale.x, 1/scale.y, 1/scale.z ) ;
#else
        res.scale = 1.0 /scale;
#endif
        res.rotate = rotate.inverse();
        res.translate = res.rotate.apply( -translate * res.scale );
        return res;
    }

    // transform COMPOSITION
    Transform operator * (const Transform & b) const; // composition
};


inline Versor3 Transform::Up() const {
    Versor3 up = Versor3::up();
    up = (*this)(up);
    return up;
}

inline void Transform::Up(Versor3& outUp) const {
    outUp = Versor3::up();
    outUp = (*this)(outUp);
}

// the result is the transformation that does b first, then me (this)
inline Transform Transform::operator * (const Transform & b) const 
{
    Transform c;
    c.scale = scale * b.scale;
    c.rotate = rotate * b.rotate;
    c.translate = translate + c.rotate.apply( b.translate * scale);

    return c;
}

inline bool areEqual(const Transform &a , const Transform &b )
{
    return areEqual( a.scale ,    b.scale )
        && areEqual( a.translate, b.translate )
        && areEquivalent( a.rotate,    b.rotate );
}

}; // end of namespace definition
