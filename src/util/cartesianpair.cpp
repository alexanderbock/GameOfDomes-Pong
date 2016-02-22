//
//  cartesianpair.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "cartesianpair.h"

#include "common.h"
#include <assert.h>
#include <math.h>

CartesianPair::CartesianPair()
    : x(0)
    , y(0)
    , z(0)
{}

CartesianPair::CartesianPair(float x_, float y_, float z_)
    : x(x_)
    , y(y_)
    , z(z_)
{}

SphericalPair CartesianPair::toSpherical() const {
    //float r = DOME_RADIUS;
    float r = sqrtf(x*x + y*y + z*z);
    float phi = atan2(y,x);
    if (abs(x) < 0.01f && abs(y) < 0.01f)
        phi = 0.f;
    float theta = acos(z/r);

    assert(abs(r - DOME_RADIUS) < 0.01f);
    const SphericalPair result(phi, theta);
    return result;
}
