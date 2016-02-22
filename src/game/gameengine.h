//
//  gameengine.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_GAMEENGINE_H__
#define __PONG_GAMEENGINE_H__

#include "common.h"
#include "game/settings.h"
#include "sgct/Engine.h"

#include <map>
#include <vector>

class BoundingBox;
class Object;
class Player;
class HavokEngine;
class hkpRigidBody;

class GameEngine {
public:
    enum InputType {
        InputTypeMoveLeft = 1,
        InputTypeMoveRight = 2,
        InputTypeRotateLeft = 4,
        InputTypeRotateRight = 8
    };

    struct GameInput {
        unsigned int player;
        std::vector<std::pair<InputType, float> > inputs;
    };

    GameEngine(int argc, char** argv, bool isMaster, sgct::Engine* engine);
    ~GameEngine();

    bool init();
    bool deinit();

    void draw();

    void setDebugMode(bool mode);
    bool debugMode() const;

    void startSimulation();
    void stopSimulation();

    void runSimulationStep();
    void updateObjects();

    std::vector<unsigned char> encodeData() const;
    void decodeData(const std::vector<unsigned char>& data);

    void applyPlayerInput(const GameInput& input);

    void foo();

   
protected:

private:
    void drawDebug() const;
    void drawBackground() const;

    void addObject(Object* object);

    void initTextures();
    void initShaders();
    void initLighting();
    void initHavok();

    void deinitTextures();
    void deinitShaders();
    void deinitHavok();

    void addSpring(hkpRigidBody* body);

    void initPlayers(const int numPlayers);
    unsigned int _currentID;
    std::map<unsigned int, Object*> _objects;
    std::vector<Player*> _players;

    float _lightPosition[3];

    sgct::Engine* _engine;
    HavokEngine* _havok;
    hkpRigidBody* _centerBody;

    Settings _settings;
    bool _isDebugMode;
    bool _isMaster;

    hkReal _lastTime;
};

#endif // __PONG_GAMEENGINE_H__
