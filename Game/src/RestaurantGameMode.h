#pragma once
#include "IGameMode.h"
#include "RunState.h"
#include "GameplayAbility/Ability.h"
#include <array>
#include <cmath>

/// Per-day simulation rules baked from the current RunState.
/// Created at the start of each GameplayScene, destroyed when it ends.
/// 
/// GameplayScene::Initialize() calls:
///   auto& gm = m_world->SetGameMode<RestaurantGameMode>(gi.runState);
/// then copies the public fields into plain Scene members for zero-cost per-frame access.
class RestaurantGameMode : public Core::IGameMode
{
public:
    std::array<float, AbilityAttributes::Count()> m_attributes = AbilityAttributes::Defaults();

    explicit RestaurantGameMode(const RunState& state)
    {
        ApplyUpgrades(state);
        ApplyBuffs(state);
    }

    void BeginPlay() override {}
    void EndPlay()   override {}

    float GetAttribute(GameAttribute attribute) const
    {
        return m_attributes[AbilityAttributes::Index(attribute)];
    }

private:
    void ApplyUpgrades(const RunState& state)
    {
        for (int id : state.activeUpgrades) {
            const UpgradeDefinition* upgrade = UpgradeRegistry::Find(id);
            if (!upgrade) continue;

            for (const AttributeModifier& modifier : upgrade->Modifiers) {
                ApplyOperation(m_attributes[AbilityAttributes::Index(modifier.Attribute)], modifier);
            }
        }
    }

    void ApplyBuffs(const RunState& state)
    {
        for (int id : state.activeBuffs) {
            const BuffDefinition* buff = BuffRegistry::Find(id);
            if (!buff) continue;

            for (const AttributeModifier& modifier : buff->Modifiers) {
                ApplyOperation(m_attributes[AbilityAttributes::Index(modifier.Attribute)], modifier);
            }
        }
    }

    void ApplyOperation(float& val, AttributeModifier modifier)
    {
        switch (modifier.Op) {
        case AttributeOp::Add:
            val += modifier.Value;
            break;
        case AttributeOp::Sub:
            val -= modifier.Value;
            break;
        case AttributeOp::Mult:
            val *= modifier.Value;
            break;
        case AttributeOp::Div:
            if (std::abs(modifier.Value) > 0.0001f) {
                val /= modifier.Value;
            }
            break;
        }
    }
};
