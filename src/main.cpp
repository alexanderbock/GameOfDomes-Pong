//
//  main.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "sgct.h"
#include "game/gameengine.h"
#include "common.h"

#include <math.h>

using namespace sgct;
using std::copy;
using std::vector;

Engine* _engine = 0;
GameEngine* _game = 0;

bool _debugMode = true;
bool _reloadConfigFile = false;

vector<unsigned char> _gameData;

void myPreSyncFunc() {
    if (_engine->isMaster()) {
        _game->runSimulationStep();
        _game->updateObjects();
        _gameData = _game->encodeData();
    }
}

void myEncodeFunc() {
    SharedData::Instance()->writeBool(_debugMode);
    SharedData::Instance()->writeBool(_reloadConfigFile);
    SharedData::Instance()->writeInt32(_gameData.size()); // int v. size_t
    if (_gameData.size() > 0)
        SharedData::Instance()->writeUCharArray(&(_gameData[0]), _gameData.size());
}

void myDecodeFunc() {
    _debugMode = SharedData::Instance()->readBool();
    _reloadConfigFile = SharedData::Instance()->readBool();

    int gameDataSize = SharedData::Instance()->readInt32();
    if (gameDataSize > 0) {
        unsigned char* gameData = SharedData::Instance()->readUCharArray(gameDataSize);
        _gameData.clear();
        _gameData.resize(gameDataSize);
        copy(gameData, gameData + gameDataSize, _gameData.begin());
    }
}

void myPostSyncPreDrawFunc() {
    if (!_engine->isMaster())
        _game->decodeData(_gameData);

    if (_reloadConfigFile) {
        MessageHandler::Instance()->print("Reloading config file\n");
        _game->deinit();
        _game->init();
        if (_engine->isMaster())
            _game->startSimulation();
        _reloadConfigFile = false;
    }
    
    _game->setDebugMode(_debugMode);
}

void myDrawFunc() {
    glPushMatrix();
    _game->draw();
    glPopMatrix();
}

void myKeyboardFunc(int key, int action) {
    if (_engine->isMaster()) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case 'H':
                    MessageHandler::Instance()->print("UP:    increase theta of debug sphere\nDOWN:  decrease theta of debug sphere\nLEFT:" 
                                                         "  decrease phi of debug sphere\nRIGHT: increase phi of debug sphere\nQ:     "
                                                         "debug mode\nA:       Reload config file\n");
                    break;
                //case GLFW_KEY_RIGHT:
                //    _game->foo();
                //    break;
                case GLFW_KEY_LEFT:
                    {
                        GameEngine::GameInput input;
                        input.player = 0;
                        input.inputs.push_back(std::make_pair(GameEngine::InputTypeMoveLeft, 1.f));
                        _game->applyPlayerInput(input);
                        break;
                    }
                case GLFW_KEY_RIGHT:
                    {
                        GameEngine::GameInput input;
                        input.player = 0;
                        input.inputs.push_back(std::make_pair(GameEngine::InputTypeMoveRight, 1.f));
                        _game->applyPlayerInput(input);
                        break;
                    }
                //case 'Z':
                //    _game->startPlayerMovement(1, MovementDirectionLeft);
                //    break;
                //case 'C':
                //    _game->startPlayerMovement(1, MovementDirectionRight);
                //    break;
                case 'Q':
                    _debugMode = !_debugMode;
                    MessageHandler::Instance()->print("Set debug mode to %i\n", _debugMode);
                    break;
                case 'A':
                    _reloadConfigFile = true;
                    break;
            }
        }
        else if (action == GLFW_RELEASE) {
            switch (key) {
                //case GLFW_KEY_LEFT:
                //case GLFW_KEY_RIGHT:
                //    //_game->stopPlayerMovement(0);
                //    break;
                //case 'Z':
                //case 'C':
                //    //_game->stopPlayerMovement(1);
                //    break;
            }
        }
    }
}

int main(int argc, char** argv) {
    _engine = new Engine(argc, argv);
    _engine->setDrawFunction(myDrawFunc);
    _engine->setPreSyncFunction(myPreSyncFunc);
    _engine->setPostSyncPreDrawFunction(myPostSyncPreDrawFunc);
    _engine->setKeyboardCallbackFunction(myKeyboardFunc);

    if (!_engine->init()) {
        delete _engine;
        return EXIT_FAILURE;
    }
    
    _game = new GameEngine(argc, argv, _engine->isMaster(), _engine);
    
    if (!_game->init()) {
        delete _game;
        return EXIT_FAILURE;
    }

    SharedData::Instance()->setEncodeFunction(myEncodeFunc);
    SharedData::Instance()->setDecodeFunction(myDecodeFunc);

    if (_engine->isMaster())
        _game->startSimulation();

    _engine->render();

    delete _game;
    delete _engine;

    return 0;
}
