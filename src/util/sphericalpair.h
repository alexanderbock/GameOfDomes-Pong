//
//  sphericalpair.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_SPHERICALPAIR_H__
#define __PONG_SPHERICALPAIR_H__

#include "cartesianpair.h"

class CartesianPair;

class SphericalPair {
public:
    SphericalPair();
    SphericalPair(float phi, float theta);

    CartesianPair toCartesian() const;
    CartesianPair toCartesian(float radius) const;

    static float worldDistanceToSpherical(float distance);

    void setPhi(float phi);
    float phi() const;

    void setTheta(float theta);
    float theta() const;

    const float* elem() const;

private:
    union {
        struct {
            float _phi;
            float _theta;
        };
        float _elem[2];
    };
};

#endif // __PONG_SPHERICALPAIR_H__
