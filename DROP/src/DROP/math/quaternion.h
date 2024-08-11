#pragma once

#include "vector3.h"
#include "versor3.h"
#include "point3.h"
#include "mat3.h"

namespace VgMath{
typedef double Scalar;

class Quaternion; // a forward declaration
inline Quaternion operator * (const Quaternion &a , const Quaternion &b);  // idem

class Quaternion{
public:
    Vector3 im;
    Scalar re;

    Quaternion(Scalar a, Scalar b, Scalar c, Scalar d):im(a,b,c),re(d) {}

    // empty construction: returns quaternion 1 + 0 * i
    Quaternion():Quaternion(0,0,0,1){}

    Quaternion(Vector3 iii, Scalar rrr):im(iii),re(rrr){}

    void conjugate(){ im = -im; }

    Quaternion conjugated() const { return Quaternion( -im, +re ); }
    Quaternion operator-()  const { return Quaternion( -im, -re ); }

    Scalar squaredNorm() const { return im.squaredNorm() + re*re; }
    Scalar norm() const { return std::sqrt(squaredNorm()); }

    // aritmetic operators "OUT OF PLACE"
    Quaternion operator / (Scalar d) const { return Quaternion(im/d,re/d); }
    Quaternion operator * (Scalar d) const { return Quaternion(im*d,re*d); }

    // aritmetic operators "IN PLACE"
    void operator /= (Scalar d){ im/=d; re/=d;}
    void operator *= (Scalar d){ im*=d; re*=d;}
    void operator += (const Quaternion &q){ im+=q.im; re+=q.re; }
    void operator -= (const Quaternion &q){ im-=q.im; re-=q.re; }

    void invert() {
        conjugate();
        operator /= ( squaredNorm() );
    }

    Quaternion inverse() const{
        return conjugated() / squaredNorm();
    }

    // apply this quaternion as a rotation to a 3D vector
    // https://www.haroldserrano.com/blog/developing-a-math-engine-in-c-implementing-quaternions
    Vector3 apply(const Vector3 &v) const{

        this->im.normalized();

        Quaternion res = 
                        (*this)
                        *
                        Quaternion(v,0) // quaternion that represents a vector v
                        *
                        this->inverse();

        assert(areEqual(res.re, 0));
        return res.im;

        // Result is the same, framerate is less WTF
        //return dot(im, v) * im
        //    + (re * re) * v
        //    + (2 * re) * cross(im, v)
        //    - cross(cross(im, v), im);
    }

    // points rotate just like vectors
    Point3 apply(const Point3 &p) const{
        return  apply( p.asVector3() ).asPoint3();
    }

    // versors rotate just like vectors
    Versor3 apply(const Versor3 &d) const{
        return  apply( d.asVector3() ).asVersor3();
    }

    static Quaternion angleAxis(Degrees angle, Versor3 axis)
    {

        if (angle == 0.0) {
            angle = EPSILON;
        }

        Scalar angleRad = deg2rad(angle);

        return Quaternion(
                    axis * std::sin(angleRad * 0.5),
                    std::cos(angleRad * 0.5)
                );
    }

    Mat3 toMatrix() const
    {
        // TODO: optimize! axes, as vectors, have a lot of 0s
        return Mat3(
           apply( Versor3::right().asVector3()  ),
           apply( Versor3::up().asVector3()  ),
           apply( Versor3::forward().asVector3() )
        );
    }

    Scalar getAngleDegree() const 
    {
        return rad2deg(getAngleRadians());
    }

    Scalar getAngleRadians() const 
    {
        Scalar halfAngle = acos(re);
        return 2 * acos(re);
    }

    Versor3 getAxis() const 
    {
        return im.normalized();
    }
};


inline Quaternion operator * (const Quaternion &a , const Quaternion &b) {
    return Quaternion(
       a.re * b.im + a.im * b.re + cross(a.im, b.im),  // imaginary part
       a.re * b.re - dot(a.im, b.im) // real part
    );
}


inline bool areEqual(const Quaternion &a , const Quaternion &b) {
    return areEqual( a.im , b.im )
        && areEqual( a.re , b.re );
}

// two quaterions a and b represent the SAME rotation iff a = b OR a = -b
inline bool areEquivalent(const Quaternion &a , const Quaternion &b) {
    return areEqual( a,  b )
        || areEqual( a, -b );
}

}; // end of namespace
