//
//  ball.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "ball.h"
#include "fence.h"
#include "sgct/ShaderManager.h"
#include "sgct/MessageHandler.h"
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/Entity/hkpRigidBodyCinfo.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

void Ball::draw() const {
    glPushMatrix();

    glTranslatef(cartesianPosition().x, cartesianPosition().y, cartesianPosition().z);
	sgct::ShaderManager::Instance()->bindShader(_shaderName);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _materialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _materialShininess);

    glutSolidSphere(_radius, 20, 20);
	sgct::ShaderManager::Instance()->unBindShader();

    glPopMatrix();
}

void Ball::encode(std::vector<unsigned char>& data) const {
    encodeType(data, id());
    encodeType(data, Ball::ClassID);
    encodeType(data, cartesianPosition());
    encodeType(data, _radius);
}

void Ball::decode(const std::vector<unsigned char>& data, size_t& pos) {
    setCartesianPosition(decodeType<glm::vec3>(data, pos));
    _radius = decodeType<float>(data, pos);
}

void Ball::setRadius(float radius) {
    _radius = radius;
}

void Ball::setShader(const std::string& shaderName) {
    _shaderName = shaderName;
}

void Ball::setMaterial(float* ambient, float* diffuse, float* specular, float shininess) {
    for (int i = 0; i < 4; ++i) {
        _materialAmbient[i] = ambient[i];
        _materialDiffuse[i] = diffuse[i];
        _materialSpecular[i] = specular[i];
    }
    _materialShininess = shininess;
}

void Ball::createHavokEntity() {
    hkpSphereShape* sphereShape = new hkpSphereShape(_radius);

    hkpRigidBodyCinfo ci;
    ci.m_shape = sphereShape;
    const glm::vec3& pos = cartesianPosition();
    ci.m_position = hkVector4(pos.x, pos.y, pos.z);
    ci.m_motionType = hkpMotion::MOTION_DYNAMIC;
    ci.m_restitution = 1.f;
    ci.m_friction = 0.f;

    hkpMassProperties massProperties;
    hkpInertiaTensorComputer::computeSphereVolumeMassProperties(_radius, mass(), massProperties);
    ci.setMassProperties(massProperties);

    _havokEntity = new hkpRigidBody(ci);
    sphereShape->removeReference();
}
