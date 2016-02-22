//
//  havokengine.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_HAVOKENGINE_H__
#define __PONG_HAVOKENGINE_H__

#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/World/hkpWorldCinfo.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>				

#include <stdio.h>

class HavokEngine {
public:
    HavokEngine(bool vdbEnabled);
    ~HavokEngine();

    void stepSimulation(float deltaTime);

    hkpWorld* getWorld() { return _pWorld; }

private:
    bool _vdbEnabled; // vdb = Visual Debugger.
    hkpPhysicsContext* _context;
    hkVisualDebugger* _vdb;

    hkJobQueue* _jobQueue;
    hkJobThreadPool* _threadPool;

    hkpWorld* _pWorld; // Physics world.

    void initHavok();
    void initMemory();
    void initPhysicsWorld();
    void initVDB();

    void deinitHavok();
    void deinitVDB();
};


#endif // __PONG_HAVOKENGINE_H__