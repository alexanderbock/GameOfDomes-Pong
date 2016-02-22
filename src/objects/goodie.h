//
//  goodie.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_GOODIE_H__
#define __PONG_GOODIE_H__

#include "object.h"

class Goodie : public Object {
public:
    static const int ClassID = 2;
    
    Goodie();

    void draw() const;
    void encode(std::vector<unsigned char>& data) const;
    void decode(const std::vector<unsigned char>& data, int& pos);
};

#endif // __PONG_OBJECT_H__
