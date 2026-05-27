#pragma once
#include <string>
#include <vector>
#include <array>

/// Identifies an upgrade's gameplay effect category.
enum class UpgradeEffect
{
    ExtraTable,         // Spawns additional table set in GameplayScene
    BetterKitchen,      // Reduces staff cook time
    DeliveryCounter,    // Enables passive delivery income
    OutdoorSeating,     // Adds outdoor seats (more capacity)
    Decoration,         // Boosts reputation cap & customer patience
    IndoorExpansion,    // Large capacity increase, unlocks 3rd table row
};

struct UpgradeData
{
    int           id          = 0;
    const char*   name        = "";
    const char*   description = "";
    UpgradeEffect effect      = UpgradeEffect::ExtraTable;
    /// Magnitude of the effect (meaning depends on UpgradeEffect)
    float         magnitude   = 1.0f;
};

namespace UpgradeRegistry
{
    inline constexpr std::array<UpgradeData, 12> k_upgrades =
    {{
        { 0,  "Extra Table",        "Add one more table and two seats.",              UpgradeEffect::ExtraTable,       1.0f },
        { 1,  "Outdoor Seating",    "Place chairs outside — +4 extra seats.",        UpgradeEffect::OutdoorSeating,   1.0f },
        { 2,  "Better Kitchen",     "Faster cooking. Staff cook time –30%.",         UpgradeEffect::BetterKitchen,    0.70f },
        { 3,  "Speed Kitchen",      "Even faster! Cook time –50% total.",            UpgradeEffect::BetterKitchen,    0.50f },
        { 4,  "Delivery Counter",   "Passive delivery orders earn $5 every 20 s.",   UpgradeEffect::DeliveryCounter,  5.0f },
        { 5,  "Cozy Decorations",   "Plants & lights. Customer patience +20%.",      UpgradeEffect::Decoration,       1.20f },
        { 6,  "Neon Sign",          "Eye-catching sign. Reputation cap raised.",     UpgradeEffect::Decoration,       1.10f },
        { 7,  "Indoor Expansion",   "Knock down a wall — 3-row table layout.",       UpgradeEffect::IndoorExpansion,  1.0f },
        { 8,  "Second Delivery",    "Two delivery slots. Passive income ×2.",        UpgradeEffect::DeliveryCounter,  10.0f },
        { 9,  "Premium Kitchen",    "Top-of-the-line gear. Cook time –70% total.",  UpgradeEffect::BetterKitchen,    0.30f },
        { 10, "Garden Patio",       "Full outdoor section — +8 seats.",              UpgradeEffect::OutdoorSeating,   2.0f },
        { 11, "Luxury Décor",       "Gold & velvet. Customers spend 50% more.",     UpgradeEffect::Decoration,       1.50f },
    }};

    inline const UpgradeData* Find(int id)
    {
        for (const auto& u : k_upgrades)
            if (u.id == id) return &u;
        return nullptr;
    }
}
