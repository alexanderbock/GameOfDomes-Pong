//
//  ball.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_BALL_H__
#define __PONG_BALL_H__

#include "object.h"

class Ball : public Object {
public:
    static const unsigned char ClassID = 0;

    void setRadius(float radius);
    void setShader(const std::string& shaderName);
    void setMaterial(float* ambient, float* diffuse, float* specular, float shininess);

    void draw() const;
    void drawDebug() const;
    void encode(std::vector<unsigned char>& data) const;
    void decode(const std::vector<unsigned char>& data, size_t& pos);

    void createHavokEntity();

private:
    float _radius;
    std::string _shaderName;
    float _materialAmbient[4];
    float _materialDiffuse[4];
    float _materialSpecular[4];
    float _materialShininess;
    unsigned int _lastPlayer;
};

#endif // __PONG_BALL_H__
