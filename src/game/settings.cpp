//
//  settings.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "common.h"
#include "game/settings.h"
#include "objects/ball.h"
#include "util/objectfactory.h"
#include "sgct/MessageHandler.h"
#include "tinyxml/tinyxml.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

using sgct::MessageHandler;
using std::map;
using std::string;
using std::vector;

const string Settings::ObjectNodeName = "Object";
const string Settings::ObjectTypeIdentifier = "type";
#ifdef DEPLOYMENT
const string Settings::DefaultSettingsFile = "config/default.xml";
#else
const string Settings::DefaultSettingsFile = "../config/default.xml";
#endif
const string Settings::WorldNodeName = "World";
const string Settings::ObjectPositionName = "Position";
const string Settings::ObjectMovementName = "Movement";
const string Settings::ObjectRadiusName = "Radius";
const string Settings::ObjectMassName = "Mass";
const string Settings::ObjectShaderName = "Shader";
const string Settings::ObjectHeightName = "Height";
const string Settings::ObjectSpeedName = "Speed";
const string Settings::ObjectMaterialName = "Material";
const string Settings::ObjectMaterialAmbientName = "K_ambient";
const string Settings::ObjectMaterialDiffuseName = "K_diffuse";
const string Settings::ObjectMaterialSpecularName = "K_specular";
const string Settings::ObjectMaterialShininessName = "Shininess";
const string Settings::ObjectSizeName = "Size";
const string Settings::MiscNodeName = "Misc";
const string Settings::NumPlayersNodeName = "NumPlayers";
const string Settings::GridColorNodeName = "GridColor";
const string Settings::BackgroundImageNodeName = "Background";
const string Settings::LightingNodeName = "Lighting";
const string Settings::ValueIdentifier = "value";
const string Settings::PhiIdentifier = "phi";
const string Settings::ThetaIdentifier = "theta";
const string Settings::RedIdentifier = "r";
const string Settings::GreenIdentifier = "g";
const string Settings::BlueIdentifier = "b";
const string Settings::AlphaIdentifier = "a";
const string Settings::RadiusIdentifier = "radius";
const string Settings::PiConstant = "c_pi";
const string Settings::PiHalfConstant = "c_pi2";
const string Settings::PiQuarterConstant = "c_pi4";
const string Settings::PiTwoConstant = "c_2pi";
const string Settings::PiThreeQuarterConstant = "c_3pi4";
const string Settings::ObjectTypeBall = "Ball";
const string Settings::ObjectTypeFence = "Fence";
const string Settings::ObjectTypeGoodie = "Goodie";
const string Settings::ObjectTypeTarget = "Target";
const string Settings::ObjectTypePaddle = "Paddle";

Settings::Settings()
    : _settingsFile("")
{}

void Settings::setSettingsFile(const std::string& settingsFile) {
    _settingsFile = settingsFile;
}

bool Settings::readSettings() {
    _indentationLevel = 1;

    bool result;
    if (_settingsFile == "") {
        MessageHandler::Instance()->print("Loading default settings...\n");
        result = readSettings(Settings::DefaultSettingsFile);
    }
    else {
        MessageHandler::Instance()->print("Loading settings from contents of file '%s'\n", _settingsFile.c_str());
        result = readSettings(_settingsFile);
    }

    MessageHandler::Instance()->print("Finished loading settings\n");
    return result;
}

bool Settings::readSettings(const std::string& settingsFile) {
    TiXmlDocument document(settingsFile);
    bool loadSuccess = document.LoadFile();
    if (!loadSuccess) {
        MessageHandler::Instance()->print("Error loading game settings file: %s\n", settingsFile.c_str());
        return false;
    }

    TiXmlElement* rootElem = document.RootElement();
    if (rootElem == 0) {
        MessageHandler::Instance()->print("No root element existed in settings file\n");
        return false;
    }

    TiXmlElement* worldElem = rootElem->FirstChildElement(WorldNodeName);
    MessageHandler::Instance()->print("Parsing world node\n");
    bool worldSuccess = parseWorldNode(worldElem);
    if (!worldSuccess)
        return false;

    TiXmlElement* miscElem = rootElem->FirstChildElement(MiscNodeName);
    MessageHandler::Instance()->print("Parsing misc node\n");
    bool miscSuccess = parseMiscObjectsNode(miscElem);
    if (!miscSuccess)
        return false;

    return true;
}

bool Settings::parseWorldNode(TiXmlElement* worldElem) {
    if (worldElem == 0) {
        MessageHandler::Instance()->print("World node did not exist\n");
        return false;
    }

    TiXmlElement* child = worldElem->FirstChildElement();
    while (child != 0) {
        ++_indentationLevel;
        bool parsingSuccess = parseObject(child);
        if (!parsingSuccess)
            return false;

        child = child->NextSiblingElement();
        --_indentationLevel;
    }

    return true;
}

bool Settings::parseMiscObjectsNode(TiXmlElement* miscElem) {
    if (miscElem == 0) {
        MessageHandler::Instance()->print("Misc node did not exist\n");
        return false;
    }

    ++_indentationLevel;
    TiXmlElement* numPlayersElem = miscElem->FirstChildElement(NumPlayersNodeName);
    if (numPlayersElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", NumPlayersNodeName.c_str());
        return false;
    }
    else {
        MessageHandler::Instance()->print("Parsing '%s'\n", NumPlayersNodeName.c_str());
        bool success = parse(numPlayersElem, ValueIdentifier, _miscInfo.numPlayers);
        if (!success)
            return false;
    }
    numPlayersElem = 0;


    TiXmlElement* gridColorElem = miscElem->FirstChildElement(GridColorNodeName);
    if (gridColorElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", NumPlayersNodeName.c_str());
        return false;
    }
    else {
        MessageHandler::Instance()->print("Parsing '%s'\n", GridColorNodeName.c_str());
        bool success = parseRGBA(gridColorElem, _miscInfo.gridColor[0], _miscInfo.gridColor[1], _miscInfo.gridColor[2], _miscInfo.gridColor[3]);
        if (!success)
            return false;
    }
    gridColorElem = 0;


    TiXmlElement* backgroundImageElem = miscElem->FirstChildElement(BackgroundImageNodeName);
    if (backgroundImageElem) {
        MessageHandler::Instance()->print("Background Image has not been implemented yet.\n");
    }
    backgroundImageElem = 0;


    TiXmlElement* lightingElem = miscElem->FirstChildElement(LightingNodeName);
    if (lightingElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", LightingNodeName.c_str());
        return false;
    }
    else {
        MessageHandler::Instance()->print("Parsing '%s'\n", LightingNodeName.c_str());
        TiXmlElement* ambientElem = lightingElem->FirstChildElement(ObjectMaterialAmbientName);
        if (ambientElem == 0) {
            MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectMaterialAmbientName.c_str());
            return false;
        }
        else {
            bool success = parseRGBA(ambientElem, _miscInfo.lighting.ambient[0], _miscInfo.lighting.ambient[1], 
                _miscInfo.lighting.ambient[2], _miscInfo.lighting.ambient[3]);
            if (!success)
                return false;
        }
        ambientElem = 0;

        TiXmlElement* positionElem = lightingElem->FirstChildElement(ObjectPositionName);
        if (positionElem == 0) {
            MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectPositionName.c_str());
            return false;
        }
        else {
            bool success = parsePhiThetaValues(positionElem, _miscInfo.lighting.phi, _miscInfo.lighting.theta);
            success &= parse(positionElem, RadiusIdentifier, _miscInfo.lighting.radius);
            
            if (!success)
                return false;
        }
    }
    lightingElem = 0;

    --_indentationLevel;

    return true;
}

bool Settings::parseObject(TiXmlElement* objectElem) {
    string value = objectElem->ValueStr();
    if (value != ObjectNodeName) {
        MessageHandler::Instance()->print("Malformed settings file. Object tree contained "
            "child with node name != '%s'\n", ObjectNodeName.c_str());
        return false;
    }

    const string* type = objectElem->Attribute(ObjectTypeIdentifier);
    if (type == 0) {
        MessageHandler::Instance()->print("%s attribute did not exist\n", ObjectTypeIdentifier.c_str());
        return false;
    }
    else {
        if (*type == ObjectTypeBall)
            return parseBall(objectElem);
        else if (*type == ObjectTypeFence)
            return parseFence(objectElem);
        else if (*type == ObjectTypeGoodie)
            return parseGoodie(objectElem);
        else if (*type == ObjectTypePaddle)
            return parsePaddle(objectElem);
        else if (*type == ObjectTypeTarget)
            return parseTarget(objectElem);
    }
    
    MessageHandler::Instance()->print("Unrecognized Object type '%s'\n", type->c_str());
    return false;
}

bool Settings::parseBall(TiXmlElement* ballElem) {
    MessageHandler::Instance()->print("Parsing '%s'\n", ObjectTypeBall.c_str());
    Ball* ball = dynamic_cast<Ball*>(ObjectFactory::createObject(ObjectTypeBall));

    ++_indentationLevel;

    TiXmlElement* positionElem = ballElem->FirstChildElement(ObjectPositionName);
    if (positionElem != 0) {
        float phi, theta;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectPositionName.c_str());
        bool positionSuccess = parsePhiThetaValues(positionElem, phi, theta);
        if (!positionSuccess)
            return false;
        else {
            ball->setSphericalPosition(glm::vec3(DOME_RADIUS, phi, theta));
        }
    }
    positionElem = 0;

    TiXmlElement* velocityElem = ballElem->FirstChildElement(ObjectMovementName);
    if (velocityElem != 0) {
        //float phi, theta;
        //MessageHandler::Instance()->print("Parsing '%s'\n", ObjectMovementName.c_str());
        //bool velocitySuccess = parsePhiThetaValues(velocityElem, phi, theta);
        //if (!velocitySuccess)
        //    return false;
        //else
        //    ball->setMovement(phi, theta);
    }
    velocityElem = 0;

    TiXmlElement* radiusElem = ballElem->FirstChildElement(ObjectRadiusName);
    if (radiusElem != 0) {
        float radius;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectRadiusName.c_str());
        bool success = parse(radiusElem, ValueIdentifier, radius);
        if (!success)
            return false;
        else
            ball->setRadius(radius);
    }
    radiusElem = 0;

    TiXmlElement* massElem = ballElem->FirstChildElement(ObjectMassName);
    if (massElem != 0) {
        float mass;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectRadiusName.c_str());
        bool success = parse(massElem, ValueIdentifier, mass);
        if (!success)
            return false;
        else
            ball->setMass(mass);
    }
    massElem = 0;

    TiXmlElement* shaderElem = ballElem->FirstChildElement(ObjectShaderName);
    if (shaderElem != 0) {
        string shaderName;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectShaderName.c_str());
        int success = shaderElem->QueryValueAttribute(ValueIdentifier, &shaderName);
        if (success == TIXML_WRONG_TYPE) {
            MessageHandler::Instance()->print("%s attribute had the wrong type\n", ObjectShaderName.c_str());
            return false;
        }
        else
            ball->setShader(shaderName);
    }
    shaderElem = 0;

    TiXmlElement* materialElem = ballElem->FirstChildElement(ObjectMaterialName);
    if (materialElem != 0) {
        float ambient[4];
        float diffuse[4];
        float specular[4];
        float shininess;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectMaterialName.c_str());
        bool success = parseMaterial(materialElem, ambient, diffuse, specular, shininess);
        if (!success)
            return false;
        else {
            ball->setMaterial(ambient, diffuse, specular, shininess);
        }
    }
    materialElem = 0;

    --_indentationLevel;

    _objects.push_back(ball);

    return true;
}

bool Settings::parseFence(TiXmlElement* fenceElem) {
    MessageHandler::Instance()->print("Parsing '%s'\n", ObjectTypeFence.c_str());

    ++_indentationLevel;
    
    TiXmlElement* sizeElem = fenceElem->FirstChildElement(ObjectSizeName);
    if (sizeElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectSizeName.c_str());
        return false;
    } else {
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectSizeName.c_str());
        bool success = parsePhiThetaValues(sizeElem, _fenceInfo.sizePhi, _fenceInfo.sizeTheta);
        if (!success)
            return false;
    }
    sizeElem = 0;

    TiXmlElement* massElem = fenceElem->FirstChildElement(ObjectMassName);
    if (massElem != 0) {
        float mass;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectRadiusName.c_str());
        bool success = parse(massElem, ValueIdentifier, mass);
        if (!success)
            return false;
        else
            _fenceInfo.mass = mass;
    }
    massElem = 0;

    TiXmlElement* shaderElem = fenceElem->FirstChildElement(ObjectShaderName);
    if (shaderElem != 0) {
        string shaderName;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectShaderName.c_str());
        int success = shaderElem->QueryValueAttribute(ValueIdentifier, &shaderName);
        if (success == TIXML_WRONG_TYPE) {
            MessageHandler::Instance()->print("%s attribute had the wrong type\n", ObjectShaderName.c_str());
            return false;
        }
        else
            _fenceInfo.shaderName = shaderName;
    }
    shaderElem = 0;

    TiXmlElement* materialElem = fenceElem->FirstChildElement(ObjectMaterialName);
    if (materialElem != 0) {
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectMaterialName.c_str());
        bool success = parseMaterial(materialElem, _fenceInfo.material.ambient, _fenceInfo.material.diffuse, _fenceInfo.material.specular, _fenceInfo.material.shininess);
        if (!success)
            return false;
    }
    materialElem = 0;

    --_indentationLevel;

    return true;
}

bool Settings::parseGoodie(TiXmlElement* /*goodieElem*/) {
    return true;
}

bool Settings::parseTarget(TiXmlElement* /*targetElem*/) {
    return true;
}

bool Settings::parsePaddle(TiXmlElement* paddleElem) {
    MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectTypePaddle.c_str());

    ++_indentationLevel;

    TiXmlElement* sizeElem = paddleElem->FirstChildElement(ObjectSizeName);
    if (sizeElem == 0) {
        MessageHandler::Instance()->printIndent(_indentationLevel, "Child node '%s' did not exist\n", ObjectSizeName.c_str());
        return false;
    } else {
        MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectSizeName.c_str());
        bool success = parsePhiThetaValues(sizeElem, _paddleInfo.sizePhi, _paddleInfo.sizeTheta);
        if (!success)
            return false;
    }
    sizeElem = 0;

    TiXmlElement* massElem = paddleElem->FirstChildElement(ObjectMassName);
    if (massElem != 0) {
        float mass;
        MessageHandler::Instance()->print("Parsing '%s'\n", ObjectRadiusName.c_str());
        bool success = parse(massElem, ValueIdentifier, mass);
        if (!success)
            return false;
        else
            _paddleInfo.mass = mass;
    }
    massElem = 0;

    TiXmlElement* shaderElem = paddleElem->FirstChildElement(ObjectShaderName);
    if (shaderElem != 0) {
        string shaderName;
        MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectShaderName.c_str());
        int success = shaderElem->QueryValueAttribute(ValueIdentifier, &shaderName);
        if (success == TIXML_WRONG_TYPE) {
            MessageHandler::Instance()->printIndent(_indentationLevel, "%s attribute had the wrong type\n", ObjectShaderName.c_str());
            return false;
        }
        else
            _paddleInfo.shaderName = shaderName;
    }
    shaderElem = 0;

    TiXmlElement* heightElem = paddleElem->FirstChildElement(ObjectHeightName);
    if (heightElem != 0) {
        MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectHeightName.c_str());
        bool success = parse(heightElem, ValueIdentifier, _paddleInfo.heightOffset);
        if (!success)
            return false;
    }
    heightElem = 0;

    TiXmlElement* speedElem = paddleElem->FirstChildElement(ObjectSpeedName);
    if (speedElem != 0) {
        MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectSpeedName.c_str());
        bool success = parse(speedElem, ValueIdentifier, _paddleInfo.speed);
        if (!success)
            return false;
    }
    speedElem = 0;

    TiXmlElement* materialElem = paddleElem->FirstChildElement(ObjectMaterialName);
    if (materialElem != 0) {
        MessageHandler::Instance()->printIndent(_indentationLevel, "Parsing '%s'\n", ObjectMaterialName.c_str());
        bool success = parseMaterial(materialElem, _paddleInfo.material.ambient, _paddleInfo.material.diffuse, _paddleInfo.material.specular, _paddleInfo.material.shininess);
        if (!success)
            return false;
    }
    materialElem = 0;

    --_indentationLevel;

    return true;
}

bool Settings::parseMaterial(TiXmlElement* objectElem, float* kAmbient, float* kDiffuse, float* kSpecular, float& specular) const {
    // testing at the same time if the arrays have the correct length
    kAmbient[0] = 0.f;
    kAmbient[1] = 0.f;
    kAmbient[2] = 0.f;
    kAmbient[3] = 0.f;
    kDiffuse[0] = 0.f;
    kDiffuse[1] = 0.f;
    kDiffuse[2] = 0.f;
    kDiffuse[3] = 0.f;
    kSpecular[0] = 0.f;
    kSpecular[1] = 0.f;
    kSpecular[2] = 0.f;
    kSpecular[3] = 0.f;
    specular = 0.f;

    TiXmlElement* ambientElem = objectElem->FirstChildElement(ObjectMaterialAmbientName);
    if (ambientElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectMaterialAmbientName.c_str());
        return false;
    }
    else {
        bool success = parseRGBA(ambientElem, kAmbient[0], kAmbient[1], kAmbient[2], kAmbient[3]);
        if (!success)
            return false;
    }

    TiXmlElement* diffuseElem = objectElem->FirstChildElement(ObjectMaterialDiffuseName);
    if (diffuseElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectMaterialDiffuseName.c_str());
        return false;
    }
    else {
        bool success = parseRGBA(diffuseElem, kDiffuse[0], kDiffuse[1], kDiffuse[2], kDiffuse[3]);
        if (!success)
            return false;
    }

    TiXmlElement* specularElem = objectElem->FirstChildElement(ObjectMaterialSpecularName);
    if (specularElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectMaterialSpecularName.c_str());
        return false;
    }
    else {
        bool success = parseRGBA(specularElem, kSpecular[0], kSpecular[1], kSpecular[2], kSpecular[3]);
        if (!success)
            return false;
    }

    TiXmlElement* shininessElem = objectElem->FirstChildElement(ObjectMaterialShininessName);
    if (shininessElem == 0) {
        MessageHandler::Instance()->print("Child node '%s' did not exist\n", ObjectMaterialShininessName.c_str());
        return false;
    }
    else {
        bool success = parse(shininessElem, ValueIdentifier, specular);
        if (!success)
            return false;
    }

    return true;
}

template<class T>
bool Settings::parse(TiXmlElement* elem, const std::string& identifier, T& value) const {
    value = T(0);
    int success = elem->QueryValueAttribute(identifier, &value);
    if (success == TIXML_WRONG_TYPE) {
        MessageHandler::Instance()->print("%s attribute had the wrong type\n", identifier.c_str());
        return false;
    }
    else if (success == TIXML_NO_ATTRIBUTE) { 
        MessageHandler::Instance()->print("%s attribute did not exist\n", identifier.c_str());
        return false;
    }
    else
        return true; 
}

bool Settings::parseAngle(TiXmlElement* elem, const std::string& identifier, float& value) const {
    int querySuccess = elem->QueryValueAttribute(identifier, &value);
    if (querySuccess == TIXML_NO_ATTRIBUTE) {
        MessageHandler::Instance()->print("%s attribute did not exist\n", identifier.c_str());
        return false;
    }
    else if (querySuccess == TIXML_WRONG_TYPE) {
        const string* str = elem->Attribute(identifier);
        if (str != 0) {
            bool success = convertConstant(*str, value);
            if (!success) {
                MessageHandler::Instance()->print("%s attribute had the wrong type\n", identifier.c_str());
                return false;
            }
        }
    }

    return true;
}


bool Settings::parsePhiThetaValues(TiXmlElement* elem, float& phi, float& theta) const {
    bool phiSuccess = parseAngle(elem, PhiIdentifier, phi);
    bool thetaSuccess = parseAngle(elem, ThetaIdentifier, theta);

    return phiSuccess && thetaSuccess;
}

bool Settings::parseRGBA(TiXmlElement* elem, float& r, float& g, float& b, float &a) const {
    bool success[4];
    success[0] = parse(elem, RedIdentifier, r);
    success[1] = parse(elem, GreenIdentifier, g);
    success[2] = parse(elem, BlueIdentifier, b);
    success[3] = parse(elem, AlphaIdentifier, a);

    return success[0] && success[1] && success[2] && success[3];
}

bool Settings::convertConstant(const std::string& str, float& value) const {
    if (str == PiConstant) {
        value = glm::pi<float>();
        return true;
    } else if (str == PiHalfConstant) {
        value = glm::half_pi<float>();
        return true;
    } else if (str == PiQuarterConstant) {
        value = glm::quarter_pi<float>();
        return true;
    } else if (str == PiTwoConstant) {
        value = 2.f * glm::pi<float>();
        return true;
    } else if (str == PiThreeQuarterConstant) {
        value = 3.f * glm::quarter_pi<float>();
        return true;
    } else {
        MessageHandler::Instance()->print("Error while converting constant '%s'\n", str.c_str());
        return false;
    }
}

const std::vector<Object*>& Settings::objects() const {
    return _objects;
}

int Settings::numberOfPlayers() const {
    return _miscInfo.numPlayers;
}

const float* Settings::gridColor() const {
    return _miscInfo.gridColor;
}

const Settings::FenceInfo& Settings::fenceInformation() const {
    return _fenceInfo;
}

const Settings::LightingInfo& Settings::lightingInformation() const {
    return _miscInfo.lighting;
}

const Settings::PaddleInfo& Settings::paddleInformation() const {
    return _paddleInfo;
}
