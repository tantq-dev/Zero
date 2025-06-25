#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "MonsterModel.h"

struct BulletDefinition {
    int id;
    std::string name;
    BulletConfig config;
    std::string textureName;

    BulletDefinition() : id(-1), name(""), textureName("") {}
    
    BulletDefinition(int bulletId, const std::string& bulletName, const BulletConfig& bulletConfig, const std::string& texName)
        : id(bulletId), name(bulletName), config(bulletConfig), textureName(texName) {}
    
    BulletDefinition(const BulletDefinition& other)
        : id(other.id), name(other.name), config(other.config), textureName(other.textureName) {}
};

class BulletRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<BulletDefinition>> m_bulletTypes;
    int m_nextBulletId = 0;

public:
    void RegisterBulletType(const std::string& name, const BulletConfig& config, const std::string& textureName);
    BulletDefinition* GetBulletType(const std::string& id);
    const std::unordered_map<std::string, std::unique_ptr<BulletDefinition>>& GetBulletTypeMap() const;
    void UpdateBulletConfig(const std::string& id, const BulletConfig& config);
    int GetNextBulletId();
    std::vector<BulletDefinition*> GetAllBulletTypes();
    void RegisterBulletTypeFromData(const std::vector<BulletDefinition>& data);
    std::vector<std::string> GetBulletTypeNames();
};