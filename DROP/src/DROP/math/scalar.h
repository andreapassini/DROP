#pragma once

#include <cmath>

namespace VgMath{

typedef double Scalar;

const Scalar EPSILON = 1e-10;
const Scalar EPSILON2 = EPSILON*EPSILON;

inline Scalar mix(Scalar a, Scalar b, Scalar t){
    return a*(1-t) + b*t;
}

inline Scalar lerp(Scalar a, Scalar b, Scalar t) {
    return a * (1 - t) + b * t;
}

inline bool areEqual( Scalar a, Scalar b ){
    return std::abs(a-b)<EPSILON;
}


// This is slower cause it uses the mod operator,
// we dont really need it since we know the max value generated
//inline Scalar randomBetween( Scalar min, Scalar max ){
//    Scalar rand0to1 = (rand() % 1001) / 1000.0;
//    return mix( min , max, rand0to1 );
//}


inline Scalar randomBetween(const Scalar min, const Scalar max) {
    Scalar random0to1 = (Scalar)std::rand() / RAND_MAX;
    return lerp(min, max, random0to1);
}

};
