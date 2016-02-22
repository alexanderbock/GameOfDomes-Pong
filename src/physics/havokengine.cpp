//
//  havokengine.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "havokengine.h"

#include <Common/Base/keycode.cxx>

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are
// present
#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#define HK_FEATURE_REFLECTION_PHYSICS
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 
#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities
#include <Common/Base/Config/hkProductFeatures.cxx>


static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit) {
    printf("%s", msg);
}

HavokEngine::HavokEngine(bool vdbEnabled) 
    : _vdbEnabled(vdbEnabled)
{
    _vdbEnabled = vdbEnabled;
    initHavok();
}

HavokEngine::~HavokEngine() {
    deinitHavok();
}

void HavokEngine::initHavok() {
    initMemory();
    initPhysicsWorld();

    if (_vdbEnabled)
        initVDB();
}

void HavokEngine::initMemory() {
    // Initialize the base system including our memory system
    // Allocate 0.5MB of physics solver buffer.
    hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo( 500* 1024 ) );
    hkBaseSystem::init( memoryRouter, errorReport );

    // We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
    // set in the following code sections.
    int totalNumThreadsUsed;

    // Get the number of physical threads available on the system
    hkHardwareInfo hwInfo;
    hkGetHardwareInfo(hwInfo);
    totalNumThreadsUsed = hwInfo.m_numThreads;

    // We use one less than this for our thread pool, because we must also use this thread for our simulation
    hkCpuJobThreadPoolCinfo threadPoolCinfo;
    threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

    // This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
    // timer collection will not be enabled.
    threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
    _threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

    // We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
    // Here we only use it for physics.
    hkJobQueueCinfo info;
    info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
    _jobQueue = new hkJobQueue(info);

    // Enable monitors for this thread.
    // Monitors have been enabled for thread pool threads already (see above comment).
    hkMonitorStream::getInstance().resize(200000);
}

void HavokEngine::initPhysicsWorld() {
    hkpWorldCinfo worldInfo;
    // Set the simulation type of the world to multi-threaded.
    worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

    worldInfo.m_gravity.set(0,0,9.81f);

    // Flag objects that fall "out of the world" to be automatically removed.
    worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
    worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);

    _pWorld = new hkpWorld(worldInfo);

    // Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
    _pWorld->m_wantDeactivation = false;

    // When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
    // to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
    // must call markForRead / markForWrite before it modifies the world to enable these checks.
    _pWorld->markForWrite();


    // Register all collision agents.
    // It's important to register collision agents before adding any entities to the world.
    hkpAgentRegisterUtil::registerAllAgents( _pWorld->getCollisionDispatcher() );

    // We need to register all modules we will be running multi-threaded with the job queue.
    _pWorld->registerWithJobQueue( _jobQueue );
}

void HavokEngine::initVDB() {
    hkArray<hkProcessContext*> contexts;

    // <PHYSICS-ONLY>: Register physics specific visual debugger processes
    // By default the VDB will show debug points and lines, however some products such as physics and cloth have additional viewers
    // that can show geometries etc and can be enabled and disabled by the VDB app.
    {
        // Initialise the visual debugger so we can connect remotely to the simulation.
        // The context must exist beyond the use of the VDB instance, and you can make
        // whatever contexts you like for your own viewer types.
        _context = new hkpPhysicsContext();
        hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
        _context->addWorld(_pWorld); // add the physics world so the viewers can see it
        contexts.pushBack(_context);

        // Now we have finished modifying the world, release our write marker.
        _pWorld->unmarkForWrite();
    }

    _vdb = new hkVisualDebugger(contexts);
    _vdb->serve();
}

void HavokEngine::deinitHavok() {
    // Cleanup physics
    _pWorld->markForWrite();
    _pWorld->removeReference();

    delete _jobQueue;

    // Clean up the thread pool
    _threadPool->removeReference();

    if (_vdbEnabled)
        deinitVDB();

    hkBaseSystem::quit();
    hkMemoryInitUtil::quit();
}

void HavokEngine::deinitVDB() {
    _vdb->removeReference();

    // Contexts are not reference counted at the base class level by the VDB as
    // they are just interfaces really. So only delete the context after you have
    // finished using the VDB.
    _context->removeReference();
}

void HavokEngine::stepSimulation(float deltaTime) {
    _pWorld->stepMultithreaded( _jobQueue, _threadPool, deltaTime);

    if (_vdbEnabled) {
        _context->syncTimers(_threadPool);
        _vdb->step();
    }

    // Clear accumulated timer data in this thread and all slave threads
    hkMonitorStream::getInstance().reset();
    _threadPool->clearTimerData();
}
