#pragma once
#include <array>
#include <cstddef>
#include <string>
#include <vector>

enum class GameAttribute {
    CustomerSpawnInterval,
    DayDurationSeconds,
    MoneyPerCustomer,
    CustomerPatience,
    CookTimeMultiplier,
    ExtraTableCount,
    HasOutdoorSeating,
    HasDeliveryCounter,
    DeliveryIncomeRate,
    ReputationBonus,
    Count // use for count and create array, do not remove
};

enum class AttributeOp {
    Mult,
    Sub,
    Add,
    Div
};

struct AttributeModifier
{
    GameAttribute Attribute = GameAttribute::CustomerSpawnInterval;
    AttributeOp Op = AttributeOp::Add;
    float Value = 0.0f;
};

struct BuffDefinition
{
    int Id = 0;
    const char* Name = "";
    const char* Description = "";
    const char* Tag = "";
    std::vector<AttributeModifier> Modifiers;
};

struct UpgradeDefinition
{
    int Id = 0;
    const char* Name = "";
    const char* Description = "";
    const char* Tag = "";
    std::vector<AttributeModifier> Modifiers;
};

namespace AbilityAttributes
{
    inline constexpr std::size_t Count()
    {
        return static_cast<std::size_t>(GameAttribute::Count);
    }

    inline constexpr std::array<float, Count()> Defaults()
    {
        std::array<float, Count()> values{};
        values[static_cast<std::size_t>(GameAttribute::CustomerSpawnInterval)] = 10.0f;
        values[static_cast<std::size_t>(GameAttribute::DayDurationSeconds)] = 120.0f;
        values[static_cast<std::size_t>(GameAttribute::MoneyPerCustomer)] = 10.0f;
        values[static_cast<std::size_t>(GameAttribute::CustomerPatience)] = 10.0f;
        values[static_cast<std::size_t>(GameAttribute::CookTimeMultiplier)] = 1.0f;
        values[static_cast<std::size_t>(GameAttribute::ExtraTableCount)] = 0.0f;
        values[static_cast<std::size_t>(GameAttribute::HasOutdoorSeating)] = 0.0f;
        values[static_cast<std::size_t>(GameAttribute::HasDeliveryCounter)] = 0.0f;
        values[static_cast<std::size_t>(GameAttribute::DeliveryIncomeRate)] = 0.0f;
        values[static_cast<std::size_t>(GameAttribute::ReputationBonus)] = 0.0f;
        return values;
    }

    inline constexpr std::size_t Index(GameAttribute attribute)
    {
        return static_cast<std::size_t>(attribute);
    }
}

namespace BuffRegistry
{
    inline const std::array<BuffDefinition, 15> k_buffs =
    {{
        {  0, "Student Discount",       "Prices cut. Customers flood in.",              "Volume",     {{ GameAttribute::CustomerSpawnInterval, AttributeOp::Div, 1.5f }} },
        {  1, "Happy Hour",             "Rush-time promo for the whole day.",           "Volume",     {{ GameAttribute::CustomerSpawnInterval, AttributeOp::Div, 1.3f }} },
        {  2, "Lunch Special",          "Midday crowd surge.",                          "Volume",     {{ GameAttribute::CustomerSpawnInterval, AttributeOp::Div, 1.2f }} },
        {  3, "Viral Food Trend",       "Your dish is all over social media.",          "Revenue",    {{ GameAttribute::MoneyPerCustomer, AttributeOp::Mult, 1.5f }} },
        {  4, "Celebrity Visit",        "A celebrity posted your food.",                "Revenue",    {{ GameAttribute::MoneyPerCustomer, AttributeOp::Mult, 2.0f }} },
        {  5, "Premium Ingredients",    "Gourmet quality earns higher checks.",         "Revenue",    {{ GameAttribute::MoneyPerCustomer, AttributeOp::Mult, 1.3f }} },
        {  6, "Michelin Buzz",          "Rumors of a star raise menu prices.",          "Revenue",    {{ GameAttribute::MoneyPerCustomer, AttributeOp::Mult, 1.8f }} },
        {  7, "Romantic Atmosphere",    "Candles and music help guests wait.",          "Patience",   {{ GameAttribute::CustomerPatience, AttributeOp::Mult, 1.5f }} },
        {  8, "Free Bread Basket",      "A small gesture buys more time.",              "Patience",   {{ GameAttribute::CustomerPatience, AttributeOp::Mult, 1.3f }} },
        {  9, "Live Music",             "A jazz trio keeps the room relaxed.",           "Patience",   {{ GameAttribute::CustomerPatience, AttributeOp::Mult, 1.6f }} },
        { 10, "Delivery App Deal",      "Delivery orders pay better today.",            "Delivery",   {{ GameAttribute::DeliveryIncomeRate, AttributeOp::Mult, 2.0f }} },
        { 11, "Influencer Partnership", "Unboxing videos boost delivery income.",       "Delivery",   {{ GameAttribute::DeliveryIncomeRate, AttributeOp::Mult, 1.5f }} },
        { 12, "Press Coverage",         "A local paper feature improves reputation.",    "Reputation", {{ GameAttribute::ReputationBonus, AttributeOp::Add, 15.0f }} },
        { 13, "5-Star Review",          "A perfect online review sets the tone.",        "Reputation", {{ GameAttribute::ReputationBonus, AttributeOp::Add, 20.0f }} },
        { 14, "Flash Sale",             "Huge crowd, but less patient customers.",       "Risk",       {{ GameAttribute::CustomerSpawnInterval, AttributeOp::Div, 1.8f }, { GameAttribute::CustomerPatience, AttributeOp::Mult, 0.7f }} },
    }};

    inline const auto& All()
    {
        return k_buffs;
    }

    inline const BuffDefinition* Find(int id)
    {
        for (const auto& buff : k_buffs) {
            if (buff.Id == id) return &buff;
        }
        return nullptr;
    }
}

namespace UpgradeRegistry
{
    inline const std::array<UpgradeDefinition, 12> k_upgrades =
    {{
        {  0, "Extra Table",      "Add one more table and two seats.",          "Capacity", {{ GameAttribute::ExtraTableCount, AttributeOp::Add, 1.0f }} },
        {  1, "Outdoor Seating",  "Place chairs outside for extra capacity.",   "Capacity", {{ GameAttribute::HasOutdoorSeating, AttributeOp::Add, 1.0f }, { GameAttribute::ExtraTableCount, AttributeOp::Add, 2.0f }} },
        {  2, "Better Kitchen",   "Faster equipment reduces cook time.",        "Kitchen",  {{ GameAttribute::CookTimeMultiplier, AttributeOp::Mult, 0.70f }} },
        {  3, "Speed Kitchen",    "Even faster prep flow.",                     "Kitchen",  {{ GameAttribute::CookTimeMultiplier, AttributeOp::Mult, 0.50f }} },
        {  4, "Delivery Counter", "Passive delivery orders earn steady money.", "Delivery", {{ GameAttribute::HasDeliveryCounter, AttributeOp::Add, 1.0f }, { GameAttribute::DeliveryIncomeRate, AttributeOp::Add, 0.25f }} },
        {  5, "Cozy Decorations", "Plants and lights make guests patient.",     "Decor",    {{ GameAttribute::CustomerPatience, AttributeOp::Mult, 1.20f }} },
        {  6, "Neon Sign",        "An eye-catching sign boosts reputation.",    "Decor",    {{ GameAttribute::ReputationBonus, AttributeOp::Add, 10.0f }} },
        {  7, "Indoor Expansion", "Knock down a wall for a larger layout.",     "Capacity", {{ GameAttribute::ExtraTableCount, AttributeOp::Add, 3.0f }} },
        {  8, "Second Delivery",  "Two delivery slots double passive orders.",  "Delivery", {{ GameAttribute::HasDeliveryCounter, AttributeOp::Add, 1.0f }, { GameAttribute::DeliveryIncomeRate, AttributeOp::Add, 0.50f }} },
        {  9, "Premium Kitchen",  "Top-tier gear dramatically cuts cook time.", "Kitchen",  {{ GameAttribute::CookTimeMultiplier, AttributeOp::Mult, 0.30f }} },
        { 10, "Garden Patio",     "A full patio adds major seating.",           "Capacity", {{ GameAttribute::HasOutdoorSeating, AttributeOp::Add, 1.0f }, { GameAttribute::ExtraTableCount, AttributeOp::Add, 4.0f }} },
        { 11, "Luxury Decor",     "Upscale ambience raises what guests spend.", "Decor",    {{ GameAttribute::MoneyPerCustomer, AttributeOp::Mult, 1.50f }} },
    }};

    inline const auto& All()
    {
        return k_upgrades;
    }

    inline const UpgradeDefinition* Find(int id)
    {
        for (const auto& upgrade : k_upgrades) {
            if (upgrade.Id == id) return &upgrade;
        }
        return nullptr;
    }

    inline bool ModifiesAttribute(const UpgradeDefinition& upgrade, GameAttribute attribute)
    {
        for (const auto& modifier : upgrade.Modifiers) {
            if (modifier.Attribute == attribute) return true;
        }
        return false;
    }
}
