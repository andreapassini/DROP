#pragma once

#include <assert.h>
#include "mat3.h"
#include "quaternion.h"
#include "point3.h"

#include "glm/glm.hpp"

namespace VgMath{

#define ANISOTROPIC_SCALING 0


/* a class for a transformation */
class Transform{
public:
    glm::vec3 translate;
#if ANISOTROPIC_SCALING
    Vector3 scale; // (NON-UNIFORM or ANISOTROPIC) scaling
#else
    float scale; // (UNIFORM or ISOTROPIC) scaling
#endif
    glm::vec4 rotate;

    // empty constructor: returns identity transform
#if ANISOTROPIC_SCALING
    Transform(): translate(0,0,0), scale(1,1,1),  rotate() {}
#else
    //Transform(): translate(0,0,0), scale(1),  rotate(VgMath::Vector3(0.0, 1.0, 0.0), 0.0) {}
    Transform(): translate(0,0,0), scale(1),  rotate() {}
#endif

//    Vector3 operator() (const Vector3& v) const{
//        return rotate.apply(scale * v);
//    }
//    Versor3 operator() (const Versor3& d) const{
//        return rotate.apply( d );
//    }
//    Point3 operator() (const Point3& p) const{
//        return rotate.apply(p.scaled(scale)) + translate;
//    }
//    Scalar operator() (const Scalar& d) const{
//        return d*scale;
//    }
//
//    // operatoins between transforms
//    Transform inverse() const{
//        Transform res;
//#if ANISOTROPIC_SCALING
//        res.scale = Vector3( 1/scale.x, 1/scale.y, 1/scale.z ) ;
//#else
//        res.scale = 1/scale;
//#endif
//        res.rotate = rotate.inverse();
//        res.translate = res.rotate.apply( -translate*res.scale );
//        return res;
//    }
//
//    // transform COMPOSITION
//    Transform operator * (const Transform & b) const; // composition
};



//// the result is the transformation that does b first, then me (this)
//inline Transform Transform::operator * (const Transform & b) const {
//    Transform c;
//    c.scale = scale * b.scale;
//    c.rotate = b.rotate * rotate;
//    c.translate = translate + rotate.apply( scale * b.translate );
//
//    return c;
//}
//
//
//inline bool areEqual(const Transform &a , const Transform &b ){
//    return areEqual( a.scale ,    b.scale )
//        && areEqual( a.translate, b.translate )
//        && areEquivalent( a.rotate,    b.rotate );
//}


}; // end of namespace definition
