#include "MonsterModel.h"
#include <string>
#include <functional>

BulletConfig::BulletConfig(const BulletConfig& bulletConfig)
{
	this->speed = bulletConfig.speed;
	this->aliveTime = bulletConfig.aliveTime;
	this->bulletType = bulletConfig.bulletType;
	this->validBulletIngame = bulletConfig.validBulletIngame;
	this->damage = bulletConfig.damage;
	this->bounce = bulletConfig.bounce;
	this->ID = bulletConfig.ID;
	if (bulletConfig.spawnerBullet)
		this->spawnerBullet = std::make_unique<SpawnerBulletConfig>(*bulletConfig.spawnerBullet);
	else
		this->spawnerBullet = nullptr;
}

BulletConfig& BulletConfig::operator=(const BulletConfig& bulletConfig)
{
	BulletConfig bullet;
	bullet.speed = bulletConfig.speed;
	bullet.aliveTime = bulletConfig.aliveTime;
	bullet.bulletType = bulletConfig.bulletType;
	bullet.validBulletIngame = bulletConfig.validBulletIngame;
	bullet.damage = bulletConfig.damage;
	bullet.bounce = bulletConfig.bounce;
	if (bulletConfig.spawnerBullet)
		bullet.spawnerBullet = std::make_unique<SpawnerBulletConfig>(*bulletConfig.spawnerBullet);
	else
		bullet.spawnerBullet = nullptr;
	return bullet;
}

std::string BulletConfig::GetBulletID(const BulletConfig& bulletConfig)
{
	return std::to_string(bulletConfig.ID);
}