//
//  settings.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_SETTINGS_H__
#define __PONG_SETTINGS_H__

#include <map>
#include <string>
#include <vector>

class Object;
class TiXmlElement;

class Settings {
public:
    struct LightingInfo {
        float ambient[4];
        float phi;
        float theta;
        float radius;
    };

    struct MaterialInfo {
        float ambient[4];
        float diffuse[4];
        float specular[4];
        float shininess;
    };

    struct FenceInfo {
        float sizePhi;
        float sizeTheta;
        float mass;
        std::string shaderName;
        MaterialInfo material;
    };

    struct PaddleInfo {
        float sizePhi;
        float sizeTheta;
        float mass;
        std::string shaderName;
        float heightOffset;
        float speed;
        MaterialInfo material;
    };

    Settings();

    void setSettingsFile(const std::string& settingsFile);
    bool readSettings();

    const std::vector<Object*>& objects() const;

    int numberOfPlayers() const;
    const float* gridColor() const;
    const FenceInfo& fenceInformation() const;
    const PaddleInfo& paddleInformation() const;
    const LightingInfo& lightingInformation() const;
    

private:
    struct MiscInfo {
        int numPlayers;
        float gridColor[4];
        LightingInfo lighting;
    };

    static const std::string WorldNodeName;
    static const std::string ObjectNodeName;
    static const std::string ObjectTypeBall;
    static const std::string ObjectTypeFence;
    static const std::string ObjectTypeGoodie;
    static const std::string ObjectTypeTarget;
    static const std::string ObjectTypePaddle;
    static const std::string ObjectTypeIdentifier;
    static const std::string ObjectPositionName;
    static const std::string ObjectMovementName;
    static const std::string ObjectRadiusName;
    static const std::string ObjectMassName;
    static const std::string ObjectShaderName;
    static const std::string ObjectHeightName;
    static const std::string ObjectSpeedName;
    static const std::string ObjectMaterialName;
    static const std::string ObjectMaterialAmbientName;
    static const std::string ObjectMaterialDiffuseName;
    static const std::string ObjectMaterialSpecularName;
    static const std::string ObjectMaterialShininessName;
    static const std::string ObjectSizeName;

    static const std::string MiscNodeName;
    static const std::string NumPlayersNodeName;
    static const std::string GridColorNodeName;
    static const std::string BackgroundImageNodeName;
    static const std::string LightingNodeName;

    static const std::string ValueIdentifier;
    static const std::string PhiIdentifier;
    static const std::string ThetaIdentifier;
    static const std::string RedIdentifier;
    static const std::string GreenIdentifier;
    static const std::string BlueIdentifier;
    static const std::string AlphaIdentifier;
    static const std::string RadiusIdentifier;

    static const std::string PiConstant;
    static const std::string PiHalfConstant;
    static const std::string PiQuarterConstant;
    static const std::string PiTwoConstant;
    static const std::string PiThreeQuarterConstant;

    bool readSettings(const std::string& settingsFile);

    bool parseWorldNode(TiXmlElement* worldElem);
    bool parseMiscObjectsNode(TiXmlElement* miscElem);
    bool parseObject(TiXmlElement* objectElem);
    bool parseBall(TiXmlElement* ballElem);
    bool parseFence(TiXmlElement* fenceElem);
    bool parseGoodie(TiXmlElement* goodieElem);
    bool parseTarget(TiXmlElement* targetElem);
    bool parsePaddle(TiXmlElement* paddleElem);

    bool parseMaterial(TiXmlElement* objectElem, float* kAmbient, float* kDiffuse, float* kSpecular, float& specular) const;
    bool parsePhiThetaValues(TiXmlElement* elem, float& phi, float& theta) const;
    bool parseRGBA(TiXmlElement* elem, float& r, float& g, float& b, float &a) const;

    template<class T>
    bool parse(TiXmlElement* elem, const std::string& identifier, T& value) const;

    bool parseAngle(TiXmlElement* elem, const std::string& identifier, float& value) const;

    bool convertConstant(const std::string& str, float& value) const;
    
    std::vector<Object*> _objects;
    FenceInfo _fenceInfo;
    PaddleInfo _paddleInfo;
    MiscInfo _miscInfo;
    
    static const std::string DefaultSettingsFile;
    std::string _settingsFile;
    unsigned int _indentationLevel;
};

#endif // __PONG_SETTINGS_H__
