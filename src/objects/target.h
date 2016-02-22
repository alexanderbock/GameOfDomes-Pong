//
//  target.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_TARGET_H__
#define __PONG_TARGET_H__

#include "object.h"

class Target : public Object {
public:
    static const int ClassID = 3;

    Target();

    void draw();
};

#endif // __PONG_TARGET_H__
