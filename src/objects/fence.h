//
//  fence.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_FENCE_H__
#define __PONG_FENCE_H__

#include "object.h"
#include "boundingbox.h"

class Fence : public Object {
public:
    static const unsigned char ClassID = 1;

    void setSize(float width, float height);
    void setMaterial(float* ambient, float* diffuse, float* specular, float shininess);

    void draw() const;
    void encode(std::vector<unsigned char>& data) const;
    void decode(const std::vector<unsigned char>& data, size_t& pos);

    void createHavokEntity();

protected:
    BoundingBox boundingBox() const;

private:
    float _width;
    float _height;
    float _materialAmbient[4];
    float _materialDiffuse[4];
    float _materialSpecular[4];
    float _materialShininess;
};

#endif // __PONG_FENCE_H__
