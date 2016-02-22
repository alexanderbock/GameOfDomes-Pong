//
//  objectfactory.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_OBJECTFACTORY_H__
#define __PONG_OBJECTFACTORY_H__

#include <string>

class Object;

class ObjectFactory {
public:
    static Object* createObject(int classID);
    static Object* createObject(const std::string& name);
};

#endif // __PONG_OBJECTFACTORY_H__
