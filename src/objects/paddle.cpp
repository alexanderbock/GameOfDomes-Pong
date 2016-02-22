//
//  paddle.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "paddle.h"
#include "common.h"
#include "sgct/ShaderManager.h"
#include "boundingbox.h"
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Dynamics/Entity/hkpRigidBodyCinfo.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Utilities/Dynamics/KeyFrame/hkpKeyFrameUtility.h>

Paddle::Paddle()
: _rotation(0)
    , _playerId(-1)
{}

void Paddle::draw() const {
    const float phiExtent = SphericalPair::worldDistanceToSpherical(_width / 2.f);
    const float thetaExtent = SphericalPair::worldDistanceToSpherical(_height / 2.f);
    const SphericalPair lowerLeftSph(sphericalPosition().y - phiExtent, sphericalPosition().z - thetaExtent);
    //const SphericalPair& lowerRight(sphericalPosition().y + phiExtent, sphericalPosition().z - thetaExtent);
    //const SphericalPair& upperLeft(sphericalPosition().y - phiExtent, sphericalPosition().z + thetaExtent);
    const SphericalPair upperRightSph(sphericalPosition().y + phiExtent, sphericalPosition().z + thetaExtent);

    const BoundingBox bbox(lowerLeftSph, upperRightSph);
    CartesianPair lowerLeft, lowerRight, upperRight, upperLeft;
    bbox.getCorners(lowerLeft, lowerRight, upperRight, upperLeft);

	sgct::ShaderManager::Instance()->bindShader( "PhongShader" );

    // -------------------------------------------
    // Material parameters:


    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _materialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _materialShininess);

    glBegin(GL_QUADS);
    	glVertex3fv(lowerLeft.elem);
    	glVertex3fv(lowerRight.elem);
    	glVertex3fv(upperRight.elem);
    	glVertex3fv(upperLeft.elem);
   glEnd();
	sgct::ShaderManager::Instance()->unBindShader();

}

void Paddle::setMaterial(float* ambient, float* diffuse, float* specular, float shininess) {
    for (int i = 0; i < 4; ++i) {
        _materialAmbient[i] = ambient[i];
        _materialDiffuse[i] = diffuse[i];
        _materialSpecular[i] = specular[i];
    }
    _materialShininess = shininess;
}

void Paddle::encode(std::vector<unsigned char>& data) const {
    encodeType(data, id());
    encodeType(data, Paddle::ClassID);
    encodeType(data, cartesianPosition());
	encodeType(data, _rotation);
}

void Paddle::decode(const std::vector<unsigned char>& data, size_t& pos) {
    setCartesianPosition(decodeType<glm::vec3>(data, pos));
	_rotation = decodeType<float>(data, pos);
}

void Paddle::setSize(float width, float height) {
    _width = width;
    _height = height;
}

void Paddle::createHavokEntity() {
    const BoundingBox& bbox = boundingBox();
    CartesianPair lowerLeft, lowerRight, upperRight, upperLeft;
    bbox.getCorners(lowerLeft, lowerRight, upperRight, upperLeft);

    const float x = abs(upperRight.x - lowerLeft.x);
    const float y = abs(upperRight.y - lowerLeft.y);
    const float z = abs(upperRight.z - lowerLeft.z);

    hkVector4 halfExtents(x / 2.f, y / 2.f, z / 2.f);
    hkpBoxShape* boxShape = new hkpBoxShape(halfExtents);

    hkpRigidBodyCinfo ci;
    ci.m_shape = boxShape;
    const glm::vec3& pos = cartesianPosition();
    ci.m_position = hkVector4(pos.x, pos.y, pos.z);
    //ci.m_motionType = hkpMotion::MOTION_DYNAMIC;
    //ci.m_motionType = hkpMotion::MOTION_FIXED;
    //ci.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
    //ci.m_motionType = hkpMotion::MOTION_CHARACTER;
    ci.m_motionType = hkpMotion::MOTION_KEYFRAMED;
    ci.m_friction = 0.f;
    ci.m_restitution = 1.f;

    hkpMassProperties massProperties;
    hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, mass(), massProperties);
    ci.setMassProperties(massProperties);

    _havokEntity = new hkpRigidBody(ci);
    boxShape->removeReference();
}

BoundingBox Paddle::boundingBox() const {
    const float phiExtent = SphericalPair::worldDistanceToSpherical(_width / 2.f);
    const float thetaExtent = SphericalPair::worldDistanceToSpherical(_height / 2.f);

    const SphericalPair lowerLeftSph(sphericalPosition().y - phiExtent, sphericalPosition().z - thetaExtent);
    const SphericalPair upperRightSph(sphericalPosition().y + phiExtent, sphericalPosition().z + thetaExtent);

    return BoundingBox(lowerLeftSph, upperRightSph);
}

void Paddle::moveLeft(float linearVelocity) {
    glm::vec3 sphPosOld = sphericalPosition();
    sphPosOld.y -= linearVelocity;
    SphericalPair sphPosNew(sphPosOld.y, sphPosOld.z);
    CartesianPair cartPosNew = sphPosNew.toCartesian();
    hkVector4 cartPos(cartPosNew.x, cartPosNew.y, cartPosNew.z);
    hkQuaternion rot;
    rot.setAxisAngle(hkVector4(0,1,0), 0.f);
    hkpKeyFrameUtility::applySoftKeyFrame(cartPos, rot, 1.f/60.f, _havokEntity);

    //const glm::vec3& pos = cartesianPosition();
    //glm::vec3 direction = glm::vec3(-sin(pos.x / DOME_RADIUS), cos(pos.y / DOME_RADIUS), 0.f);
    //_havokEntity->applyLinearImpulse(hkVector4(direction.x * linearVelocity, direction.y * linearVelocity, 0.f));
}

void Paddle::moveRight(float linearVelocity) {
    glm::vec3 sphPosOld = sphericalPosition();
    sphPosOld.y += linearVelocity;
    SphericalPair sphPosNew(sphPosOld.y, sphPosOld.z);
    CartesianPair cartPosNew = sphPosNew.toCartesian();
    hkVector4 cartPos(cartPosNew.x, cartPosNew.y, cartPosNew.z);
    hkQuaternion rot;
    rot.setAxisAngle(hkVector4(0,1,0), 0.f);
    hkpKeyFrameUtility::applySoftKeyFrame(cartPos, rot, 1.f/60.f, _havokEntity);
    //const glm::vec3& pos = cartesianPosition();
    //glm::vec3 direction = glm::vec3(-sin(pos.x / DOME_RADIUS), cos(pos.y / DOME_RADIUS), 0.f);
    //_havokEntity->applyLinearImpulse(hkVector4(direction.x * -linearVelocity, direction.y * -linearVelocity, 0.f));
}
