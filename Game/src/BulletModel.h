#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
//#include "json.hpp"
//using json = nlohmann::json;

// Forward declarations
class IBulletMoveBehavior;

// Movement behavior base class
class IBulletMoveBehavior {
public:
	virtual ~IBulletMoveBehavior() = default;
	virtual std::string GetType() const = 0;
	virtual IBulletMoveBehavior* Clone() const = 0;

};

// Concrete movement behavior classes
class BulletStraight : public IBulletMoveBehavior {
public:
	std::string GetType() const override { return "Bullet_Straight"; }
	IBulletMoveBehavior* Clone() const override { return new BulletStraight(*this); }
};

class BulletParabol : public IBulletMoveBehavior {
public:
	std::string GetType() const override { return "Bullet_Parabol"; }
	IBulletMoveBehavior* Clone() const override { return new BulletParabol(*this); }
};

class BulletMortar : public IBulletMoveBehavior {
public:
	std::string GetType() const override { return "Bullet_Mortar"; }
	IBulletMoveBehavior* Clone() const override { return new BulletMortar(*this); }
};

// Factory class for creating movement behaviors
class MoveBehaviorFactory {
public:
	static std::unique_ptr<IBulletMoveBehavior> CreateMoveBehavior(const std::string& type) {
		if (type == "Bullet_Straight") {
			return std::make_unique<BulletStraight>();
		}
		else if (type == "Bullet_Parabol") {
			return std::make_unique<BulletParabol>();
		}
		else if (type == "Bullet_Mortar") {
			return std::make_unique<BulletMortar>();
		}
		else {
			return std::make_unique<BulletStraight>(); // Default fallback
		}
	};
};



// Bullet configuration struct
//struct BulletConfig {
//	std::string ID;
//	std::string AssetID;
//	float MoveSpeed;
//	int Damage;
//	float AliveTime;
//	std::string Elemental;
//	std::unique_ptr<IBulletMoveBehavior> MoveBehavior;
//	int Bounce;
//
//	// Default constructor
//	BulletConfig()
//		: MoveSpeed(0.0f), Damage(0), AliveTime(0.0f), Bounce(0) {
//	}
//
//	// Constructor with parameters
//	BulletConfig(const std::string& id, const std::string& assetId,
//		float moveSpeed, int damage, float aliveTime,
//		const std::string& elemental,
//		std::unique_ptr<IBulletMoveBehavior> moveBehavior,
//		int bounce)
//		: ID(id), AssetID(assetId), MoveSpeed(moveSpeed), Damage(damage),
//		AliveTime(aliveTime), Elemental(elemental),
//		MoveBehavior(std::move(moveBehavior)), Bounce(bounce) {
//	}
//
//	// Copy constructor
//	BulletConfig(const BulletConfig& other)
//		: ID(other.ID), AssetID(other.AssetID), MoveSpeed(other.MoveSpeed),
//		Damage(other.Damage), AliveTime(other.AliveTime),
//		Elemental(other.Elemental), Bounce(other.Bounce) {
//		if (other.MoveBehavior) {
//			MoveBehavior = std::unique_ptr<IBulletMoveBehavior>(other.MoveBehavior->Clone());
//		}
//	}
//
//	// Assignment operator
//	BulletConfig& operator=(const BulletConfig& other) {
//		if (this != &other) {
//			ID = other.ID;
//			AssetID = other.AssetID;
//			MoveSpeed = other.MoveSpeed;
//			Damage = other.Damage;
//			AliveTime = other.AliveTime;
//			Elemental = other.Elemental;
//			Bounce = other.Bounce;
//
//			if (other.MoveBehavior) {
//				MoveBehavior = std::unique_ptr<IBulletMoveBehavior>(other.MoveBehavior->Clone());
//			}
//			else {
//				MoveBehavior.reset();
//			}
//		}
//		return *this;
//	}
//
//	// Move constructor
//	BulletConfig(BulletConfig&& other) noexcept
//		: ID(std::move(other.ID)), AssetID(std::move(other.AssetID)),
//		MoveSpeed(other.MoveSpeed), Damage(other.Damage),
//		AliveTime(other.AliveTime), Elemental(std::move(other.Elemental)),
//		MoveBehavior(std::move(other.MoveBehavior)), Bounce(other.Bounce) {
//	}
//
//	// Move assignment operator
//	BulletConfig& operator=(BulletConfig&& other) noexcept {
//		if (this != &other) {
//			ID = std::move(other.ID);
//			AssetID = std::move(other.AssetID);
//			MoveSpeed = other.MoveSpeed;
//			Damage = other.Damage;
//			AliveTime = other.AliveTime;
//			Elemental = std::move(other.Elemental);
//			MoveBehavior = std::move(other.MoveBehavior);
//			Bounce = other.Bounce;
//		}
//		return *this;
//	}
//
//	void to_json(json& j, const BulletConfig& config) {
//		j = json{
//	   {"ID", config.ID},
//	   {"AssetID", config.AssetID},
//	   {"MoveSpeed", config.MoveSpeed},
//	   {"Damage", config.Damage},
//	   {"AliveTime", config.AliveTime},
//	   {"Elemental", config.Elemental},
//	   {"MoveBehavior", {{"type", config.MoveBehavior->GetType()}}},
//	   {"Bounce", config.Bounce}
//		};
//	}
//	void from_json(const json& j, BulletConfig& config) {
//		j.at("ID").get_to(config.ID);
//		j.at("AssetID").get_to(config.AssetID);
//		j.at("MoveSpeed").get_to(config.MoveSpeed);
//		j.at("Damage").get_to(config.Damage);
//		j.at("AliveTime").get_to(config.AliveTime);
//		j.at("Elemental").get_to(config.Elemental);
//		j.at("Bounce").get_to(config.Bounce);
//
//		std::string behaviorType;
//		j.at("MoveBehavior").at("type").get_to(behaviorType);
//		config.MoveBehavior = MoveBehaviorFactory::CreateMoveBehavior(behaviorType);
//	}
//};