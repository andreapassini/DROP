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
    // q.apply(v) = q' * v * q
    Vector3 apply(const Vector3 &v) const{
        // TODO: optimize! exploiting the 0 of the real part
        Quaternion res = this->conjugated()
                         *
                         Quaternion(v,0) // quaternion that represents a vector v
                         *
                         (*this);
        assert( areEqual( res.re , 0 ) );
        return res.im;
    }

    // points rotate just like vectors
    Point3 apply(const Point3 &p) const{
        return  apply( p.asVector3() ).asPoint3();
    }

    // versors rotate just like vectors
    Versor3 apply(const Versor3 &d) const{
        return  apply( d.asVector3() ).asVersor3();
    }

    static Quaternion angleAxis(Degrees angle, Versor3 axis){
        Scalar angleRad = deg2rad(angle);
        return Quaternion(
            std::sin( angleRad / 2 ) * axis,
            std::cos( angleRad / 2 )
        );
    }

    static Quaternion angleAxis(Degrees angle, Versor3&& axis) {
        Scalar angleRad = deg2rad(angle);
        return Quaternion(
            std::sin(angleRad / 2) * axis,
            std::cos(angleRad / 2)
        );
    }

    Mat3 toMatrix() const{
        // TODO: optimize! axes, as vectors, have a lot of 0s
        return Mat3(
           apply( Versor3::right().asVector3()  ),
           apply( Versor3::up().asVector3()  ),
           apply( Versor3::forward().asVector3() )
        );
    }

    Scalar getAngleDegree() {
        return rad2deg(getAngleRadians());
    }

    Scalar getAngleRadians() {
        //assert(this->norm() <= 1.0);
        //assert(re >= -1 && re <= 1);

        return 2 * acos(re);
    }

    Vector3 getAxis() {
        Scalar angle = getAngleRadians();

        if (angle == 0.0)
            return im;

        im / angle;
        return im;
    }
};


inline Quaternion operator * (const Quaternion &a , const Quaternion &b) {
    return Quaternion(
       a.re*b.im + a.im*b.re + cross(a.im,b.im),  // imaginary part
       a.re*b.re - dot(a.im,b.im) // real part
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
