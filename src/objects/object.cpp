//
//  object.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "object.h"

#include <limits>

#include "sgct/MessageHandler.h"
#include <Physics/Dynamics/Entity/hkpRigidBody.h>

Object::Object()
    : _id(std::numeric_limits<unsigned int>::max())
    , _havokEntity(0)
    , _cartesianPosition(0.f, 0.f, 0.f)
    , _sphericalPosition(0.f, 0.f, 0.f)
{}

unsigned int Object::id() const {
    return _id;
}

void Object::setID(unsigned int id) {
    _id = id;
}

void Object::setCartesianPosition(const glm::vec3& position) {
    _cartesianPosition = position;

    const float r = glm::length(position);
    float phi = atan2(position.y, position.x);
    if (abs(position.x) < 0.01f && abs(position.y) < 0.01f)
        phi = 0.f;
    const float theta = acos(position.z / r);

    //assert(abs(r - DOME_RADIUS) < 0.1f);
    _sphericalPosition.x = r;
    _sphericalPosition.y = phi;
    _sphericalPosition.z = theta;
}

void Object::setSphericalPosition(const glm::vec3& position) {
    //assert(abs(position.x - DOME_RADIUS) < 0.01f);
    _sphericalPosition = position;

    const float x = position.x * cos(position.y) * sin(position.z);
    const float y = position.x * sin(position.y) * sin(position.z);
    const float z = position.x * cos(position.z);

    _cartesianPosition.x = x;
    _cartesianPosition.y = y;
    _cartesianPosition.z = z;
}


const glm::vec3& Object::cartesianPosition() const {
    return _cartesianPosition;
}

const glm::vec3& Object::sphericalPosition() const {
    return _sphericalPosition;
}

void Object::setMass(float mass) {
    _mass = mass;
}

float Object::mass() const {
    return _mass;
}

hkpRigidBody* Object::havokEntity() const {
    assert(_havokEntity);
    return _havokEntity;
}

void Object::updateFromHavok() {
    assert(_havokEntity);
    const hkVector4& pos = _havokEntity->getPosition();
    const glm::vec3& p = glm::vec3(pos.getComponent(0), pos.getComponent(1), pos.getComponent(2));
    setCartesianPosition(p);
}
