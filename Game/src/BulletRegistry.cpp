#include "BulletRegistry.h"
#include "utilities/Logger.h"

void BulletRegistry::RegisterBulletType(const std::string& name, const BulletConfig& config, const std::string& textureName)
{
    int bulletId = GetNextBulletId();
    m_bulletTypes.insert_or_assign(std::to_string(bulletId), std::make_unique<BulletDefinition>(bulletId, name, config, textureName));
}

BulletDefinition* BulletRegistry::GetBulletType(const std::string& id)
{
    auto it = m_bulletTypes.find(id);
    if (it != m_bulletTypes.end())
    {
        return it->second.get();
    }
    return nullptr;
}

const std::unordered_map<std::string, std::unique_ptr<BulletDefinition>>& BulletRegistry::GetBulletTypeMap() const
{
    return m_bulletTypes;
}

void BulletRegistry::UpdateBulletConfig(const std::string& id, const BulletConfig& config)
{
    LOG_INFO("UpdateBulletConfig ");
    auto it = m_bulletTypes.find(id);
    if (it != m_bulletTypes.end())
    {
        it->second->config = config;
    }
    else
    {
        // Handle the case where the bullet type does not exist
        LOG_INFO("Bullet type not found: " + id);
    }
}

int BulletRegistry::GetNextBulletId()
{
    return m_nextBulletId++;
}

std::vector<BulletDefinition*> BulletRegistry::GetAllBulletTypes()
{
    std::vector<BulletDefinition*> types;
    types.reserve(m_bulletTypes.size());

    for (const auto& pair : m_bulletTypes) {
        types.push_back(pair.second.get());
    }
    return types;
}

void BulletRegistry::RegisterBulletTypeFromData(const std::vector<BulletDefinition>& data)
{
    for (const auto& bulletType : data)
    {
        if (bulletType.id >= m_nextBulletId)
        {
            m_nextBulletId = bulletType.id + 1;
        }
        m_bulletTypes.insert_or_assign(std::to_string(bulletType.id), std::make_unique<BulletDefinition>(bulletType));
    }
}

 std::vector<std::string> BulletRegistry::GetBulletTypeNames()
{
    std::vector<std::string> names;
    names.reserve(m_bulletTypes.size());
    for (const auto& pair : m_bulletTypes)
    {
        names.push_back(pair.second->name);
    }
    return names;
}
