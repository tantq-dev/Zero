#pragma once
#include "IGameMode.h"
#include "RunState.h"
#include "UpgradeRegistry.h"
#include "BuffRegistry.h"

/// Per-day simulation rules baked from the current RunState.
/// Created at the start of each GameplayScene, destroyed when it ends.
/// 
/// GameplayScene::Initialize() calls:
///   auto& gm = m_world->SetGameMode<RestaurantGameMode>(gi.runState);
/// then copies the public fields into plain Scene members for zero-cost per-frame access.
class RestaurantGameMode : public Core::IGameMode
{
public:
    // ---- Simulation parameters (read by GameplayScene on init) ----
    float customerSpawnInterval = 10.0f;  ///< Seconds between customer spawns
    float dayDurationSeconds    = 120.0f; ///< Real-time seconds per in-game day
    int   moneyPerCustomer      = 10;     ///< Base payout when customer leaves satisfied
    float customerPatience      = 10.0f;  ///< How long before an unserved customer rage-quits
    float cookTimeMultiplier    = 1.0f;   ///< Staff cook time multiplier (1.0 = normal)
    int   extraTableCount       = 0;      ///< Number of additional table sets to spawn
    bool  hasOutdoorSeating     = false;  ///< Whether outdoor seat row is spawned
    bool  hasDeliveryCounter    = false;  ///< Whether passive delivery income is active
    float deliveryIncomeRate    = 0.0f;   ///< Passive $ per second from delivery
    int   reputationBonus       = 0;      ///< Flat rep added at day start
    float spawnMultiplierEvent  = 1.0f;   ///< Runtime override from event choices
    float patienceMultiplierEvent= 1.0f;  ///< Runtime override from event choices

    explicit RestaurantGameMode(const RunState& state)
    {
        ApplyUpgrades(state);
        ApplyBuffs(state);
    }

    void BeginPlay() override {}
    void EndPlay()   override {}

private:
    void ApplyUpgrades(const RunState& state)
    {
        for (int uid : state.activeUpgrades)
        {
            const UpgradeData* u = UpgradeRegistry::Find(uid);
            if (!u) continue;

            switch (u->effect)
            {
            case UpgradeEffect::ExtraTable:
                extraTableCount += static_cast<int>(u->magnitude);
                break;
            case UpgradeEffect::OutdoorSeating:
                hasOutdoorSeating = true;
                extraTableCount  += static_cast<int>(u->magnitude);
                break;
            case UpgradeEffect::BetterKitchen:
                cookTimeMultiplier *= u->magnitude; // magnitude < 1 = faster
                break;
            case UpgradeEffect::DeliveryCounter:
                hasDeliveryCounter = true;
                deliveryIncomeRate += u->magnitude; // $/s added
                break;
            case UpgradeEffect::Decoration:
                customerPatience      *= u->magnitude;
                break;
            case UpgradeEffect::IndoorExpansion:
                extraTableCount += 3; // large layout upgrade
                break;
            }
        }
    }

    void ApplyBuffs(const RunState& state)
    {
        for (int bid : state.activeBuffs)
        {
            const BuffData* b = BuffRegistry::Find(bid);
            if (!b) continue;

            switch (b->effect)
            {
            case BuffEffect::FasterSpawn:
                customerSpawnInterval /= b->magnitude;
                break;
            case BuffEffect::SlowerSpawn:
                customerSpawnInterval *= b->magnitude;
                break;
            case BuffEffect::MoreMoney:
                moneyPerCustomer = static_cast<int>(moneyPerCustomer * b->magnitude);
                break;
            case BuffEffect::MorePatience:
                customerPatience *= b->magnitude;
                break;
            case BuffEffect::LessPatience:
                customerPatience *= b->magnitude;
                break;
            case BuffEffect::ReputationBoost:
                reputationBonus += static_cast<int>(b->magnitude);
                break;
            case BuffEffect::DeliveryBoost:
                deliveryIncomeRate *= b->magnitude;
                break;
            case BuffEffect::SpawnHighRoller:
                // Handled by GameplayScene spawn logic using this flag
                moneyPerCustomer = static_cast<int>(moneyPerCustomer * b->magnitude);
                customerSpawnInterval *= 1.2f; // fewer but richer
                break;
            case BuffEffect::SpawnImpatient:
                customerSpawnInterval /= b->magnitude; // more customers
                customerPatience      *= 0.7f;          // but impatient
                break;
            }
        }

        // Clamp spawn interval to sane range
        if (customerSpawnInterval < 2.0f)  customerSpawnInterval = 2.0f;
        if (customerSpawnInterval > 30.0f) customerSpawnInterval = 30.0f;
    }
};
