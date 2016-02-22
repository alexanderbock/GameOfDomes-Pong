//
//  sphericalpair.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "sphericalpair.h"

#include "common.h"
#include <assert.h>
#include <math.h>

SphericalPair::SphericalPair()
    : _phi(0.f)
    , _theta(0.f)
{}

SphericalPair::SphericalPair(float phi, float theta)
    : _phi(0.f)
    , _theta(0.f)
{
    setPhi(phi);
    setTheta(theta);
}

CartesianPair SphericalPair::toCartesian() const {
    float x = DOME_RADIUS * sin(_theta) * cos(_phi);
    float y = DOME_RADIUS * sin(_theta) * sin(_phi);
    float z = DOME_RADIUS * cos(_theta);

    const CartesianPair result(x,y,z);
    return result;
}

CartesianPair SphericalPair::toCartesian(float radius) const {
    float x = radius * DOME_RADIUS * sin(_theta) * cos(_phi);
    float y = radius * DOME_RADIUS * sin(_theta) * sin(_phi);
    float z = radius * DOME_RADIUS * cos(_theta);
    const CartesianPair result(x,y,z);

    return result;
}

float SphericalPair::worldDistanceToSpherical(float distance) {
    // law of cosines applied to the isosceles triangle (DOME_RADIUS, DOME_RADIUS, distance)
    //return acos(1 - ( (distance* distance) / (DOME_RADIUS * DOME_RADIUS) ));

    // law of arc lengths
    return (distance / DOME_RADIUS) / 2.f;
}

void SphericalPair::setPhi(float phi) {
    //if (phi < 0.f) {
    //    _phi = phi + 2 * M_PI;
    //    return true;
    //}
    //else if (phi > 2 * M_PI) {
    //    _phi = phi - 2 * M_PI;
    //    return true;
    //}
    //else {
        _phi = phi;
    //    return false;
    //}
}

float SphericalPair::phi() const {
    return _phi;
}

void SphericalPair::setTheta(float theta) {
    //if (theta > M_PI) {
    //    const float delta = theta - _theta;
    //    _theta = M_PI - (delta - (M_PI - _theta));
    //    setPhi(phi() + M_PI);
    //    return true;
    //}
    //else {
        _theta = theta;
        //return false;
    //}
}

float SphericalPair::theta() const {
    return _theta;
}

const float* SphericalPair::elem() const {
    return &(_elem[0]);
}