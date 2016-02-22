//
//  boundingbox.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "boundingbox.h"

#include "common.h"

using std::max;
using std::min;

BoundingBox::BoundingBox() {
    pMin = SphericalPair(std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    pMax = SphericalPair(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
}

BoundingBox::BoundingBox(const SphericalPair& p)
    : pMin(p), pMax(p)
{}

BoundingBox::BoundingBox(const SphericalPair& p1, const SphericalPair& p2) {
    pMin = SphericalPair(min(p1.phi(), p2.phi()), min(p1.theta(), p2.theta()));
    pMax = SphericalPair(max(p1.phi(), p2.phi()), max(p1.theta(), p2.theta()));
}

void BoundingBox::getCorners(CartesianPair& lowerLeft, CartesianPair& lowerRight,
                             CartesianPair& upperRight, CartesianPair& upperLeft) const
{
    const SphericalPair lowerLeftSph = pMin;
    lowerLeft = lowerLeftSph.toCartesian();

    const SphericalPair lowerRightSph(pMax.phi(), pMin.theta());
    lowerRight = lowerRightSph.toCartesian();

    const SphericalPair upperRightSph = pMax;
    upperRight = upperRightSph.toCartesian();

    const SphericalPair upperLeftSph(pMin.phi(), pMax.theta());
    upperLeft = upperLeftSph.toCartesian();
}


bool BoundingBox::overlaps(const BoundingBox &b) const {
    bool x = (pMax.phi() >= b.pMin.phi()) && (pMin.phi() <= b.pMax.phi());
    bool y = (pMax.theta() >= b.pMin.theta()) && (pMin.theta() <= b.pMax.theta());
    return (x && y);
}

bool BoundingBox::isInside(const SphericalPair &pt) const {
    return (pt.phi() >= pMin.phi() && pt.phi() <= pMax.phi() &&
        pt.theta() >= pMin.theta() && pt.theta() <= pMax.theta());
}

void BoundingBox::draw() const {
    CartesianPair lowerLeft, lowerRight, upperRight, upperLeft;
    getCorners(lowerLeft, lowerRight, upperRight, upperLeft);

    glLineWidth(3.f);
    glBegin(GL_LINE_STRIP);
    glVertex3fv(lowerLeft.elem);
    glVertex3fv(lowerRight.elem);
    glVertex3fv(upperRight.elem);
    glVertex3fv(upperLeft.elem);
    glVertex3fv(lowerLeft.elem);
    glEnd();
    glLineWidth(1.f);
}
