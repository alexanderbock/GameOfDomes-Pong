//
//  gameengine.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "gameengine.h"
#include "game/player.h"
#include "game/settings.h"
#include "common.h"
#include "objects/object.h"
#include "objects/ball.h"
#include "objects/fence.h"
#include "objects/goodie.h"
#include "objects/paddle.h"
#include "objects/target.h"
#include "util/boundingbox.h"
#include "util/objectfactory.h"
#include "physics/havokengine.h"
#include "physics/paddleheightaction.h"

#include "sgct/Engine.h"
#include "sgct/MessageHandler.h"
#include "sgct/ShaderManager.h"
#include "sgct/TextureManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Dynamics/Entity/hkpRigidBodyCinfo.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/Constraint/Bilateral/StiffSpring/hkpStiffSpringConstraintData.h>
#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>

#include <assert.h>
#include <math.h>

using std::map;
using std::vector;
using sgct::MessageHandler;
using sgct::ShaderManager;
using sgct::TextureManager;

GameEngine::GameEngine(int argc, char** argv, bool isMaster, sgct::Engine* engine)
    : _engine(engine)
    , _currentID(0)
    , _havok(0)
    , _centerBody(0)
    , _isDebugMode(false)
    , _isMaster(isMaster)
    , _lastTime(0)
{
    if (argc != 1 && argc != 3) {
        MessageHandler::Instance()->print("Wrong number of arguments (%i)\n", argc);
        return;
    }

    if (argc == 3 && (argv[1] == "-settings"))
        _settings.setSettingsFile(argv[2]);
}

GameEngine::~GameEngine() {
    deinit();
}

bool GameEngine::init() {
    bool success = _settings.readSettings();
    if (!success)
        return false;

    if (_isMaster)
        initHavok();

    initTextures();
    initShaders();
    initLighting();
    initPlayers(2);

    const vector<Object*>& objects = _settings.objects();
    vector<Object*>::const_iterator it = objects.begin();
    for (it; it != objects.end(); ++it)
        addObject(*it);

    return true;
}

bool GameEngine::deinit() {
    map<unsigned int, Object*>::iterator objectIt = _objects.begin();
    for (objectIt; objectIt != _objects.end(); ++objectIt)
        delete objectIt->second;
    _objects.clear();

    vector<Player*>::iterator playerIt = _players.begin();
    for (playerIt; playerIt != _players.end(); ++playerIt)
        delete *playerIt;
    _players.clear();

    _settings = Settings();

    deinitShaders();
    deinitTextures();

    if (_isMaster)
        deinitHavok();

    return true;
}

hkpRigidBody* ball;

void GameEngine::addObject(Object* object) {
    // needs to be thread safe w.r.t. simulator
    object->setID(_currentID);
    ++_currentID;

    assert(_objects.find(object->id()) == _objects.end());
    _objects[object->id()] = object;

    if (_isMaster) {
        _havok->getWorld()->lock();
        object->createHavokEntity();
        hkpRigidBody* havokEntity = object->havokEntity();
        _havok->getWorld()->addEntity(havokEntity);
        addSpring(havokEntity);

        //if (dynamic_cast<Paddle*>(object)) {
        //    PaddleHeightAction* paddleHeight = new PaddleHeightAction(havokEntity, object->cartesianPosition().z);
        //    _havok->getWorld()->addAction(paddleHeight);
        //    paddleHeight->removeReference();
        //}

        _havok->getWorld()->unlock();

        if (dynamic_cast<Ball*>(object))
            ball = havokEntity;
    }
}

vector<unsigned char> GameEngine::encodeData() const {
    // needs to be thread safe w.r.t. simulator
    vector<unsigned char> result;
    map<unsigned int, Object*>::const_iterator it = _objects.begin();
    for (it; it != _objects.end(); ++it) {
        Object* obj = it->second;
        obj->encode(result);
    }
    return result;
}

void GameEngine::decodeData(const std::vector<unsigned char>& data) {
    size_t position = 0;
    while (position < data.size()) {
        unsigned int id = decodeType<unsigned int>(data, position);

        if (_objects.find(id) == _objects.end()) {

        }
        else {
            Object* obj = _objects[id];
            // skip the class byte
            ++position;
            obj->decode(data, position);
        }
    }

}

void GameEngine::setDebugMode(bool mode) {
    _isDebugMode = mode;
}

bool GameEngine::debugMode() const {
    return _isDebugMode;
}

void GameEngine::draw() {
    drawBackground();

    if (debugMode())
        drawDebug();

    glLightfv(GL_LIGHT0, GL_POSITION, _lightPosition);
    glEnable(GL_LIGHTING);

    map<unsigned int, Object*>::const_iterator it = _objects.begin();
    for (it; it != _objects.end(); ++it) {
        const Object* obj = it->second;
        obj->draw();
    }

    glDisable(GL_LIGHTING);
}

void GameEngine::drawBackground() const {
    glColor4fv(_settings.gridColor());
    glutWireSphere(DOME_RADIUS, 20, 20);
    GLUquadricObj *qObj = gluNewQuadric();
    gluQuadricNormals(qObj, GLU_SMOOTH);
    gluQuadricTexture(qObj, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sgct::TextureManager::Instance()->getTextureByName("BackgroundTexture"));
    gluSphere(qObj, 30.0f, 24, 24);
    glDisable(GL_TEXTURE_2D);
}

void GameEngine::drawDebug() const {
    map<unsigned int, Object*>::const_iterator it = _objects.begin();
    for (it; it != _objects.end(); ++it) {
        const Object* obj = it->second;
        obj->draw();
    }

    // coordinate system
    glLineWidth(4.f);
    glBegin(GL_LINES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(5.f, 0.f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 5.f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 5.f);
    glEnd();

    // equator
    glColor3f(1.f, 1.f, 0.f);
    float x = DOME_RADIUS * cos(359 * glm::pi<float>()/180.0f);
    float z = DOME_RADIUS * sin(359 * glm::pi<float>()/180.0f);
    glBegin(GL_LINES);
    for(int j = 0; j < 360; j++) {
        glVertex3f(x,z,0);
        x = DOME_RADIUS * cos(j * glm::pi<float>()/180.0f);
        z = DOME_RADIUS * sin(j * glm::pi<float>()/180.0f);
        glVertex3f(x,z,0);
    }
    glEnd();
}


void GameEngine::startSimulation() {
}

void GameEngine::initTextures() {
	//sgct::TextureManager::Instance()->setAnisotropicFilterSize(4.0f);
#ifdef DEPLOYMENT
    sgct::TextureManager::Instance()->loadTexure("BackgroundTexture", "assets/textures/box.png", true);
#else
    sgct::TextureManager::Instance()->loadTexure("BackgroundTexture", "../assets/textures/box.png", true);
#endif
}

void GameEngine::initShaders() {
	//For now use one shader for all objects
#ifdef DEPLOYMENT
    ShaderManager::Instance()->addShader( "PhongShader", "assets/shaders/phong.vert", "assets/shaders/phong.frag" );
#else
    ShaderManager::Instance()->addShader( "PhongShader", "../assets/shaders/phong.vert", "../assets/shaders/phong.frag" );
#endif
}

void GameEngine::initLighting() {
    const SphericalPair lightingSpherical(_settings.lightingInformation().phi, _settings.lightingInformation().theta);
    const CartesianPair lightingCartesian = lightingSpherical.toCartesian(_settings.lightingInformation().radius);
    _lightPosition[0] = lightingCartesian.x;
    _lightPosition[1] = lightingCartesian.y;
    _lightPosition[2] = lightingCartesian.z;

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	// Light model parameters:
	// -------------------------------------------

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _settings.lightingInformation().ambient);
}

void GameEngine::initPlayers(const int numPlayers) {
    assert(numPlayers > 1);

    Player* player1 = new Player;
    player1->setBorder(0, glm::pi<float>());
    _players.push_back(player1);

    Player* player2 = new Player;
    player2->setBorder(glm::pi<float>(), 0);
    _players.push_back(player2);

    Settings::FenceInfo fenceInfo = _settings.fenceInformation();
    Settings::PaddleInfo paddleInfo = _settings.paddleInformation();

    Paddle* paddle1 = new Paddle;
    paddle1->setSphericalPosition(glm::vec3(DOME_RADIUS, glm::half_pi<float>(), glm::half_pi<float>() + paddleInfo.heightOffset));
    paddle1->setSize(paddleInfo.sizePhi, paddleInfo.sizeTheta);
    paddle1->setMaterial(paddleInfo.material.ambient, paddleInfo.material.diffuse, paddleInfo.material.specular, paddleInfo.material.shininess);
    paddle1->setMass(paddleInfo.mass);
    //paddle1->setMovementSpeed(paddleInfo.speed);
    addObject(paddle1);
    player1->setPaddle(paddle1);

    Paddle* paddle2 = new Paddle;
    paddle2->setSphericalPosition(glm::vec3(DOME_RADIUS, 3.f * glm::half_pi<float>(), glm::half_pi<float>() + paddleInfo.heightOffset));;
    paddle2->setSize(paddleInfo.sizePhi, paddleInfo.sizeTheta);
    paddle2->setMaterial(paddleInfo.material.ambient, paddleInfo.material.diffuse, paddleInfo.material.specular, paddleInfo.material.shininess);
    paddle2->setMass(paddleInfo.mass);
    //paddle2->setMovementSpeed(paddleInfo.speed);
    addObject(paddle2);
    player2->setPaddle(paddle2);


    Fence* fence1 = new Fence;
    fence1->setSphericalPosition(glm::vec3(DOME_RADIUS, 0.f, glm::half_pi<float>()));
    fence1->setSize(fenceInfo.sizePhi, fenceInfo.sizeTheta);
    fence1->setMaterial(fenceInfo.material.ambient, fenceInfo.material.diffuse, fenceInfo.material.specular, fenceInfo.material.shininess);
    fence1->setMass(fenceInfo.mass);
    addObject(fence1);

    Fence* fence2 = new Fence;
    fence2->setSphericalPosition(glm::vec3(DOME_RADIUS, glm::pi<float>(), glm::half_pi<float>()));
    fence2->setSize(_settings.fenceInformation().sizePhi, _settings.fenceInformation().sizeTheta);
    fence2->setMaterial(fenceInfo.material.ambient, fenceInfo.material.diffuse, fenceInfo.material.specular, fenceInfo.material.shininess);
    fence2->setMass(fenceInfo.mass);
    addObject(fence2);
}

void GameEngine::initHavok() {
    _havok = new HavokEngine(true);

    _havok->getWorld()->lock();
    hkVector4 halfExtents(0.01f, 0.01f, 0.01f);
    hkpBoxShape* shape = new hkpBoxShape(halfExtents);
    hkpRigidBodyCinfo ci;
    ci.m_shape = shape;
    ci.m_motionType = hkpMotion::MOTION_FIXED;
    hkpMassProperties massProperties;
    hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, 1.f, massProperties);
    ci.setMassProperties(massProperties);

    _centerBody = new hkpRigidBody(ci);
    shape->removeReference();

    _havok->getWorld()->addEntity(_centerBody);
    _havok->getWorld()->unlock();
}

void GameEngine::deinitTextures() {
    TextureManager::Instance()->Destroy();
}

void GameEngine::deinitShaders() {
    ShaderManager::Instance()->removeShader("PhongShader");
}

void GameEngine::deinitHavok() {
    delete _havok;
    _havok = 0;
}

void GameEngine::addSpring(hkpRigidBody* body) {
    hkpStiffSpringConstraintData* spring = new hkpStiffSpringConstraintData;
    spring->setInBodySpace(hkVector4(0.f, 0.f, 0.f), hkVector4(0.f, 0.f, 0.f), DOME_RADIUS);
    //spring->setInWorldSpace(rigidBody->getTransform(), _origin->getTransform(), hkVector4(0.f, 1.0f, 0.f), hkVector4(0.f, 0.f, 0.f));
    hkpConstraintInstance* constraint = new hkpConstraintInstance(body, _centerBody, spring);
    _havok->getWorld()->addConstraint(constraint);
    constraint->removeReference();
}

void GameEngine::foo() {
    _havok->getWorld()->lock();
    ball->applyLinearImpulse(hkVector4(0.f, 0.f, -10.f));
    _havok->getWorld()->unlock();
}

void GameEngine::runSimulationStep() {
    const sgct_core::Statistics& statistics = _engine->getStatistics();
    const double& fps = statistics.getAvgFPS();
    if (fps > 1.0)
        _havok->stepSimulation(1.f / fps);
}

void GameEngine::updateObjects() {
    _havok->getWorld()->lock();
    map<unsigned int, Object*>::const_iterator it = _objects.begin();
    for (it; it != _objects.end(); ++it)
        it->second->updateFromHavok();
    _havok->getWorld()->unlock();
}

void GameEngine::applyPlayerInput(const GameInput& input) {
    assert(input.player < static_cast<int>(_players.size()));
    Player* player = _players[input.player];
    Paddle* paddle = player->paddle();

    const float baseLinearVelocity = 1.f;
    const float baseAngularVelocity = 1.f;
    _havok->getWorld()->lock();
    vector<std::pair<InputType, float> >::const_iterator it = input.inputs.begin();
    for (it; it != input.inputs.end(); ++it) {
        switch (it->first) {
            case InputTypeMoveLeft:
                paddle->moveLeft(baseLinearVelocity * it->second);
                break;
            case InputTypeMoveRight:
                paddle->moveRight(baseLinearVelocity * it->second);
                break;
            case InputTypeRotateLeft:
                break;
            case InputTypeRotateRight:
                break;
        }
    }
    _havok->getWorld()->unlock();
}


//void GameEngine::startPlayerMovement(int playerID, MovementDirection direction) {
//    assert(playerID < static_cast<int>(_players.size()));
//    Player* player = _players[playerID];
//    player->paddle()->setMovementDirection(direction);
//}
//
//void GameEngine::stopPlayerMovement(int playerID) {
//    assert(playerID < static_cast<int>(_players.size()));
//    Player* player = _players[playerID];
//    player->paddle()->setMovementDirection (MovementDirectionNone);
//}

