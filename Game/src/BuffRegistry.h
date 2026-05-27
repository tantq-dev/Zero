#pragma once
#include <string>
#include <array>

/// Identifies a buff's gameplay effect category.
enum class BuffEffect
{
    FasterSpawn,        // Customers arrive more frequently
    SlowerSpawn,        // Fewer but wealthier customers
    MoreMoney,          // Each customer pays more
    MorePatience,       // Customers wait longer before leaving
    LessPatience,       // Customers are impatient (harder mode)
    ReputationBoost,    // Start day with bonus reputation
    DeliveryBoost,      // Delivery passive income multiplied
    SpawnHighRoller,    // More high-roller customer type
    SpawnImpatient,     // More impatient customer type
};

struct BuffData
{
    int        id          = 0;
    const char* name       = "";
    const char* description= "";
    BuffEffect  effect     = BuffEffect::MoreMoney;
    float       magnitude  = 1.0f; ///< Multiplier or flat bonus depending on effect
};

namespace BuffRegistry
{
    inline constexpr std::array<BuffData, 15> k_buffs =
    {{
        // --- Volume buffs ---
        {  0, "Student Discount",      "Prices cut — customers flood in. Spawn rate ×1.5.",       BuffEffect::FasterSpawn,    1.5f  },
        {  1, "Happy Hour",            "Rush-time promo. Spawn rate ×1.3 for the whole day.",     BuffEffect::FasterSpawn,    1.3f  },
        {  2, "Lunch Special",         "Midday crowd surge. Extra spawn rate ×1.2.",              BuffEffect::FasterSpawn,    1.2f  },

        // --- Quality / money buffs ---
        {  3, "Viral Food Trend",      "Your dish is all over social media. Revenue ×1.5.",       BuffEffect::MoreMoney,      1.5f  },
        {  4, "Celebrity Visit",       "A celeb posted your food. Revenue ×2.0 today.",           BuffEffect::MoreMoney,      2.0f  },
        {  5, "Premium Ingredients",   "Gourmet quality. Customers pay 30% more.",                BuffEffect::MoreMoney,      1.3f  },
        {  6, "Michelin Buzz",         "Rumours of a star. Revenue ×1.8, spawns high-rollers.",   BuffEffect::SpawnHighRoller, 1.8f  },

        // --- Patience / retention buffs ---
        {  7, "Romantic Atmosphere",   "Candles & music. Customer patience ×1.5.",                BuffEffect::MorePatience,   1.5f  },
        {  8, "Free Bread Basket",     "Small gesture. Patience ×1.3.",                           BuffEffect::MorePatience,   1.3f  },
        {  9, "Live Music",            "Jazz trio tonight. Patience ×1.6.",                       BuffEffect::MorePatience,   1.6f  },

        // --- Passive / delivery buffs ---
        { 10, "Delivery App Deal",     "Delivery orders doubled for the day.",                    BuffEffect::DeliveryBoost,  2.0f  },
        { 11, "Influencer Partnership","Unboxing video goes live. Delivery income ×1.5.",         BuffEffect::DeliveryBoost,  1.5f  },

        // --- Reputation buffs ---
        { 12, "Press Coverage",        "Local paper feature. Start with +15 reputation.",         BuffEffect::ReputationBoost, 15.0f },
        { 13, "5-Star Review",         "Perfect online review. Start with +20 reputation.",       BuffEffect::ReputationBoost, 20.0f },

        // --- Risky / double-edged buffs ---
        { 14, "Flash Sale",            "Huge crowd but impatient. Spawn ×1.8, patience ×0.7.",   BuffEffect::SpawnImpatient,  1.8f  },
    }};

    inline const BuffData* Find(int id)
    {
        for (const auto& b : k_buffs)
            if (b.id == id) return &b;
        return nullptr;
    }
}
