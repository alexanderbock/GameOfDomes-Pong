//
//  object.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_OBJECT_H__
#define __PONG_OBJECT_H__

#include "common.h"
#include "glm/glm.hpp"

class hkpRigidBody;

class Object {
public:
    Object();

    virtual void draw() const = 0;
    virtual void encode(std::vector<unsigned char>& data) const = 0;
    virtual void decode(const std::vector<unsigned char>& data, size_t& pos) = 0;
    virtual void createHavokEntity() = 0;
    hkpRigidBody* havokEntity() const;

    void setID(unsigned int id);
    unsigned int id() const;

    void setMass(float mass);
    float mass() const;

    const glm::vec3& cartesianPosition() const;
    void setCartesianPosition(const glm::vec3& position);
    const glm::vec3& sphericalPosition() const;
    void setSphericalPosition(const glm::vec3& position);

    void updateFromHavok();

protected:
    unsigned int _id;
    hkpRigidBody* _havokEntity;

private:
    float _mass;
    glm::vec3 _cartesianPosition;
    glm::vec3 _sphericalPosition;
};

#endif // __PONG_OBJECT_H__
