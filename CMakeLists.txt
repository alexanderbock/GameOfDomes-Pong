#Game of Thrones Build Script
project(Game_Of_Domes_Pong)

cmake_minimum_required(VERSION 2.6.3 FATAL_ERROR)
set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
  )

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(GOD_DEPLOYMENT "Deployment Mode" OFF)

add_definitions("/EHsc")
  
#set(SGCT_INCLUDE_DIR "C:/Program Files (x86)/SGCT/SGCT_0.8_x86/include" CACHE PATH "SGCT include dir") 
set(SGCT_INCLUDE_DIR "D:/Development/sgct/include")

include_directories(${SGCT_INCLUDE_DIR})
link_directories(${SGCT_LIB_DIR})

set(GLM_ROOT_DIR "ext")
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories("D:/Development/hk2011_3_1_r1/Source/")
set(HAVOK_LIBS
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkBase.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkCompat.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkGeometryUtilities.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkInternal.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkSceneData.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkSerialize.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkVisualize.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpConstraintSolver.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpCollide.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpDynamics.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpInternal.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpUtilities.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkpVehicle.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkcdCollide.lib"
    "D:/Development/hk2011_3_1_r1/Lib/win32_net_9-0/debug_multithreaded/hkcdInternal.lib"
    )

if(WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/ext/glut")
    #set(SGCT_LIB_DIR "C:/Program Files (x86)/SGCT/SGCT_0.8_x86/lib/msvc10" CACHE PATH "SGCT lib dir") 
    set(SGCT_LIB_DIR "D:/Development/sgct/lib/msvc9")
    set(LIBS ${LIBS} ${SGCT_LIB_DIR}/sgct32_d.lib ${CMAKE_SOURCE_DIR}/ext/glut/lib/win32/glut32.lib)
else(WIN32) #APPLE
    set(SGCT_LIB_DIR "." CACHE PATH "SGCT lib dir") 
    find_package(GLUT REQUIRED)
    find_package(OpenGL REQUIRED)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    SET(GL_LIBS ${OPENGL_gl_LIBRARY})
    SET(GLUT_glut_LIBRARY "-framework GLUT" CACHE STRING "GLUT library for OSX")
    set(LIBS ${LIBS} sgct.a ${GLUT_glut_LIBRARY} ${COCOA_LIBRARY} ${GL_LIBS} tinyxml.a)
endif(WIN32)

if (GOD_DEPLOYMENT)
    add_definitions(-DDEPLOYMENT)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/ext)
add_subdirectory(ext/tinyxml)
#link_directories(${CMAKE_BINARY_DIR}/lib)
#set(LIBS ${LIBS} tinyxml.a) 

# link_libraries(${LIBS})

IF(WIN32)
set(CMAKE_CXX_FLAGS "-D __WIN32__")
ENDIF(WIN32)
IF(APPLE)
set(CMAKE_CXX_FLAGS "-D __APPLE__")
ENDIF(APPLE)

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories(${CMAKE_SOURCE_DIR}/src/objects)
include_directories(${CMAKE_SOURCE_DIR}/src/util)
include_directories(${CMAKE_SOURCE_DIR}/src/game)

set(SOURCE
    src/main.cpp
    src/game/gameengine.cpp
    src/game/player.cpp
    src/game/settings.cpp
    src/objects/ball.cpp
    src/objects/fence.cpp
    src/objects/goodie.cpp
    src/objects/object.cpp
    src/objects/paddle.cpp
    src/objects/target.cpp
    src/physics/havokengine.cpp
    src/util/boundingbox.cpp
    src/util/cartesianpair.cpp
    src/util/objectfactory.cpp
    src/util/sphericalpair.cpp
    )
    
set(HEADER
    src/game/gameengine.h
    src/game/player.h
    src/game/settings.h
    src/objects/ball.h
    src/objects/fence.h
    src/objects/goodie.h
    src/objects/object.h
    src/objects/paddle.h
    src/objects/target.h
    src/physics/havokengine.h
    src/util/boundingbox.h
    src/util/cartesianpair.h
    src/util/objectfactory.h
    src/util/sphericalpair.h
    )

add_executable(GOD_Pong ${SOURCE} ${HEADER})
target_link_libraries(GOD_Pong ${LIBS} tinyxml ${OPENGL_LIBRARY} ${HAVOK_LIBS})
