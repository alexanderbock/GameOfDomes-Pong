//
//  common.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_COMMON_H__
#define __PONG_COMMON_H__

#include <stdlib.h>

#if (__APPLE__)
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

#include <Common/Base/hkBase.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <limits>
#include <math.h>
#include <vector>

#define DOME_RADIUS 7.4f

template<class T>
void encodeType(std::vector<unsigned char>& data, T encoded) {
	unsigned char* p = (unsigned char*)&encoded;
	data.insert(data.end(),p,p+sizeof(T));
}

template<class T>
T decodeType(const std::vector<unsigned char>& data, size_t& pos) {
	T result;
	unsigned char* p = (unsigned char*)&result;

	std::copy(&(data[pos]), &(data[pos])+sizeof(T), p);
    pos += sizeof(T);

	return result;
}

#endif /* __PONG_COMMON_H__ */
