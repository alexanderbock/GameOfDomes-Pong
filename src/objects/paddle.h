//
//  paddle.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_PADDLE_H__
#define __PONG_PADDLE_H__

#include "object.h"
#include "sphericalpair.h"
#include "boundingbox.h"

class Paddle : public Object {
public:
    static const unsigned char ClassID = 4;

    Paddle();

    void draw() const;
    void encode(std::vector<unsigned char>& data) const;
    void decode(const std::vector<unsigned char>& data, size_t& pos);
    void setMaterial(float* ambient, float* diffuse, float* specular, float shininess);

    void setSize(float width, float height);

    void moveLeft(float linearVelocity);
    void moveRight(float linearVelocity);

    void createHavokEntity();
    BoundingBox boundingBox() const;

private:
    float _width;
    float _height;
    float _materialAmbient[4];
    float _materialDiffuse[4];
    float _materialSpecular[4];
    float _materialShininess;
    float _rotation;
    float _movementSpeed;
    int _playerId;
};

#endif // __PONG_PADDLE_H__
