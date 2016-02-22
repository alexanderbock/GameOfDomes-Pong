//
//  fence.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "fence.h"
#include "util/boundingbox.h"
#include "sgct/ShaderManager.h"
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Dynamics/Entity/hkpRigidBodyCinfo.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>

void Fence::draw() const {
    const BoundingBox bbox = boundingBox();

    CartesianPair lowerLeft, lowerRight, upperRight, upperLeft;
    bbox.getCorners(lowerLeft, lowerRight, upperRight, upperLeft);

   	sgct::ShaderManager::Instance()->bindShader( "PhongShader" );

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

void Fence::encode(std::vector<unsigned char>& data) const {
    encodeType(data, id());
    encodeType(data, Fence::ClassID);
    encodeType(data, _width);
    encodeType(data, _height);
}

void Fence::decode(const std::vector<unsigned char>& data, size_t& pos) {
    _width = decodeType<float>(data, pos);
    _height = decodeType<float>(data, pos);
}

void Fence::setSize(float width, float height) {
    _width = width;
    _height = height;
}

void Fence::setMaterial(float* ambient, float* diffuse, float* specular, float shininess) {
    for (int i = 0; i < 4; ++i) {
        _materialAmbient[i] = ambient[i];
        _materialDiffuse[i] = diffuse[i];
        _materialSpecular[i] = specular[i];
    }
    _materialShininess = shininess;
}

void Fence::createHavokEntity() {
    const BoundingBox& bbox = boundingBox();
    CartesianPair lowerLeft, lowerRight, upperRight, upperLeft;
    bbox.getCorners(lowerLeft, lowerRight, upperRight, upperLeft);

    const float x = std::max(abs(upperRight.x - lowerLeft.x), 0.01f);
    const float y = std::max(abs(upperRight.y - lowerLeft.y), 0.01f);
    const float z = std::max(abs(upperRight.z - lowerLeft.z), 0.01f);

    hkVector4 halfExtents(x / 2.f, y / 2.f, z / 2.f);
    hkpBoxShape* boxShape = new hkpBoxShape(halfExtents);

    hkpRigidBodyCinfo ci;
    ci.m_shape = boxShape;
    const glm::vec3& pos = cartesianPosition();
    ci.m_position = hkVector4(pos.x, pos.y, pos.z);
    ci.m_motionType = hkpMotion::MOTION_FIXED;
    ci.m_friction = 0.f;
    ci.m_restitution = 1.f;

    hkpMassProperties massProperties;
    hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, mass(), massProperties);
    ci.setMassProperties(massProperties);

    _havokEntity = new hkpRigidBody(ci);
    boxShape->removeReference();
}

BoundingBox Fence::boundingBox() const {
    const float phiExtent = SphericalPair::worldDistanceToSpherical(_width / 2.f);
    const float thetaExtent = SphericalPair::worldDistanceToSpherical(_height / 2.f);

    const SphericalPair lowerLeftSph(sphericalPosition().y - phiExtent, sphericalPosition().z - thetaExtent);
    const SphericalPair upperRightSph(sphericalPosition().y + phiExtent, sphericalPosition().z + thetaExtent);

    return BoundingBox(lowerLeftSph, upperRightSph);
}
