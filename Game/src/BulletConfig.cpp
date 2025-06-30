#include "MonsterModel.h"
#include <string>
#include <functional>
#include "utilities/Logger.h"  

BulletConfig::BulletConfig(const BulletConfig& bulletConfig)
{
	this->name = bulletConfig.name;
	this->speed = bulletConfig.speed;
	this->aliveTime = bulletConfig.aliveTime;
	this->bulletType = bulletConfig.bulletType;
	this->validBulletIngame = bulletConfig.validBulletIngame;
	this->damage = bulletConfig.damage;
	this->bounce = bulletConfig.bounce;
	if (bulletConfig.spawnerBullet)
		this->spawnerBullet = std::make_unique<SpawnerBulletConfig>(*bulletConfig.spawnerBullet);
	else
		this->spawnerBullet = nullptr;
}

BulletConfig& BulletConfig::operator=(const BulletConfig& bulletConfig)
{
	// Debug: Log what we're receiving
    LOG_INFO("Assignment operator called:");
    LOG_INFO("  Source Name: '" + bulletConfig.name + "'");
    LOG_INFO("  Source Speed: " + std::to_string(bulletConfig.speed));
    LOG_INFO("  Source AliveTime: " + std::to_string(bulletConfig.aliveTime));
    LOG_INFO("  Source Damage: " + std::to_string(bulletConfig.damage));
    
    // Check for self-assignment
    if (this == &bulletConfig) {
        return *this;
    }
    
    // Assign to THIS object
    this->name = bulletConfig.name;
    this->speed = bulletConfig.speed;
    this->aliveTime = bulletConfig.aliveTime;
    this->bulletType = bulletConfig.bulletType;
    this->validBulletIngame = bulletConfig.validBulletIngame;
    this->damage = bulletConfig.damage;
    this->bounce = bulletConfig.bounce;
    
    if (bulletConfig.spawnerBullet)
        this->spawnerBullet = std::make_unique<SpawnerBulletConfig>(*bulletConfig.spawnerBullet);
    else
        this->spawnerBullet = nullptr;
    
    // Debug: Log what we assigned
    LOG_INFO("After assignment:");
    LOG_INFO("  This Name: '" + this->name + "'");
    LOG_INFO("  This Speed: " + std::to_string(this->speed));
    LOG_INFO("  This AliveTime: " + std::to_string(this->aliveTime));
    LOG_INFO("  This Damage: " + std::to_string(this->damage));
    
    return *this;
}


BulletConfig BulletConfig::clone()
{
	return BulletConfig();
}
