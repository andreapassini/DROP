#pragma once

#include <cmath>

namespace VgMath{

typedef double Scalar;

const Scalar EPSILON = 1e-10;
const Scalar EPSILON2 = EPSILON*EPSILON;

inline Scalar mix(Scalar a, Scalar b, Scalar t){
    return a*(1-t) + b*t;
}

inline bool areEqual( Scalar a, Scalar b ){
    return std::abs(a-b)<EPSILON;
}


inline Scalar randomBetween( Scalar min, Scalar max ){
    Scalar rand0to1 = (rand() % 1001) / 1000.0;
    return mix( min , max, rand0to1 );
}

};
