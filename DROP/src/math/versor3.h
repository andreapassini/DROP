/* file "Versor3.h" */

#pragma once

#include <iostream>
#include <assert.h>
#include "vector3.h"




using namespace std;

namespace VgMath{

typedef double Scalar;


class Versor3{
private:

    // constuctor for intrnal use only: I cannot initialize a verson giving the coords!
    Versor3(Scalar _x, Scalar _y, Scalar _z):x(_x),y(_y),z(_z){ }

    // empty constructor
    Versor3(){ }

public:
    Scalar x,y,z; // with the understanding that x^2 + y^2 + z^2 == 1.0

    static Versor3 right()   { return Versor3(+1,0,0); }
    static Versor3 up()      { return Versor3(0,+1,0); }
    static Versor3 forward() { return Versor3(0,0,+1); }
    static Versor3 left()    { return Versor3(-1,0,0); }
    static Versor3 down()    { return Versor3(0,-1,0); }
    static Versor3 backward(){ return Versor3(0,0,-1); }

    // accessing the coordiantes as an array  READING THEM
    Scalar operator[] (int i) const {
        if (i==0) return x;
        if (i==1) return y;
        assert(i==2);
        return z;
    }


    void print() const {
        std::cout << "<" << x << "," << y << "," << z << "> "; // <> for versors (made-up convention)
    }

    // vector sum
    Vector3 operator + ( const Versor3 & b ) const {
        return Vector3( x+b.x , y+b.y , z+b.z );
    }

    // the BINARY minus
    Vector3 operator - ( const Versor3 & b ) const {
        return Vector3( x-b.x , y-b.y , z-b.z );
    }

    // the UNARY minus
    Versor3 operator - () const {
        return Versor3( -x, -y, -z );
    }

    // the UNITARY plus
    Versor3 operator + () const { return *this; }

    Vector3 operator * ( Scalar k ) const {
        return Vector3( k*x , k*y , k*z );
    }

    Vector3 operator / ( Scalar k ) const {
        return Vector3( x/k , y/k , z/k );
    }

    // makes sure this is actually UNIT LENGHT!
    void renormalize() {
        Scalar norm = sqrt( x*x + y*y + z*z );
        x /= norm;
        y /= norm;
        z /= norm;
    }

    Vector3 asVector3() const {  return Vector3(x,y,z); }


    //friend class Vector3;
    friend Versor3 Vector3::asVersor3() const;
    friend Versor3 Vector3::normalized() const;

}; // end of class Versor3

inline Vector3 operator * ( Scalar k, const Versor3 &a ){
    return a*k;
}

inline Scalar dot(const Versor3& a, const Versor3& b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vector3 cross(const Versor3& a, const Versor3& b){
    return Vector3(
       a.y*b.z - b.y*a.z, // the X of the cross
       a.z*b.x - b.z*a.x, // the Y of the cross
       a.x*b.y - b.x*a.y  // the Z of the cross
    );
}


inline Vector3 mix(const Versor3 &a, const Versor3 &b, Scalar t){
    return a*(1-t) + b*t;
}


/*inline Vector3 nlerp(Versor3 a, Versor3 b, Scalar t){
    // TODO: FIX ME
    //return a*(1-t) + b*t;
}*/


inline Scalar angleBetween( const Versor3 &a , const Versor3 &b ) {
    Scalar abSinAlpha = cross(a,b).norm(); // ab is a.norm() * b.norm()
    Scalar abCosAlpha = dot(a,b);
    return rad2deg(atan2(abSinAlpha,abCosAlpha));
}

inline bool areEqual( const Versor3& a, const Versor3& b ){
    return (a-b).isZero();
}

// DEFINIZNIONE DI Vector3::normalized();
inline Versor3 Vector3::normalized() const
{
    Scalar n = norm();
    return Versor3( x/n, y/n, z/n );
}



} // end of namespace VgMath

