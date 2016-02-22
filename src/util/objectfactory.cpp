//
//  objectfactory.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "objectfactory.h"
#include "objects/object.h"
#include "objects/ball.h"
#include "objects/fence.h"
#include "objects/goodie.h"
#include "objects/paddle.h"
#include "objects/target.h"
#include <stdexcept>

Object* ObjectFactory::createObject(const std::string& name) {
    if (name == "Ball")
        return createObject(Ball::ClassID);
    else if (name == "Fence")
        return createObject(Fence::ClassID);
    else if (name == "Goodie")
        return createObject(Goodie::ClassID);
    else if (name == "Target")
        return createObject(Target::ClassID);
    else if (name == "Paddle")
        return createObject(Paddle::ClassID);
    else
        throw std::runtime_error("Unrecognized Object ID");
}

Object* ObjectFactory::createObject(int classID) {
    switch (classID) {
        case Ball::ClassID:
            return new Ball;
            break;
        case Fence::ClassID:
            return new Fence;
            break;
        case Goodie::ClassID:
            return 0;
            //return new Goodie;
            break;
        case Target::ClassID:
            return 0;
            //return new Target;
            break;
        case Paddle::ClassID:
            return new Paddle;
            break;
        default:
            throw std::runtime_error("Unrecognized Object ID");
    }
}
