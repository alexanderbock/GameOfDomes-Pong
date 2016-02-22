//
//  boundingbox.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_BOUNDINGBOX_H__
#define __PONG_BOUNDINGBOX_H__

#include "util/sphericalpair.h"

class BoundingBox {
public:
    // BBox Public Methods
    BoundingBox();
    BoundingBox(const SphericalPair &p);
    BoundingBox(const SphericalPair &p1, const SphericalPair &p2);

    void getCorners(CartesianPair& lowerLeft, CartesianPair& lowerRight,
        CartesianPair& upperRight, CartesianPair& upperLeft) const;

    bool overlaps(const BoundingBox &b) const;
    bool isInside(const SphericalPair &pt) const;

    void draw() const;

    SphericalPair pMin, pMax;
};

#endif // __PONG_BOUNDINGBOX_H__
