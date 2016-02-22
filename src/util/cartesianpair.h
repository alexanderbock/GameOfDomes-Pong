//
//  cartesianpair.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_CARTESIANPAIR_H__
#define __PONG_CARTESIANPAIR_H__

#include "sphericalpair.h"

class SphericalPair;

class CartesianPair {
public:
    CartesianPair();
    CartesianPair(float x_, float y_, float z_);

    union {
        struct {
            float x;
            float y;
            float z;
        };
        float elem[3];
    };


    SphericalPair toSpherical() const;
};

#endif // __PONG_CARTESIANPAIR_H__
