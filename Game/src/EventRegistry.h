#pragma once
#include <array>

/// Consequence of choosing one option in a random event.
struct EventConsequence
{
    int   moneyDelta      = 0;    ///< Immediate money change
    int   reputationDelta = 0;    ///< Immediate reputation change
    float spawnMultiplier = 1.0f; ///< Customer spawn rate multiplier for rest of day
    float patienceMultiplier = 1.0f; ///< Customer patience multiplier for rest of day
    const char* outcomeText = ""; ///< Short feedback string shown in UI
};

struct EventData
{
    int          id           = 0;
    const char*  name         = "";
    const char*  description  = "";  ///< Situation description shown to player
    const char*  choiceAText  = "";  ///< Label for option A button
    const char*  choiceBText  = "";  ///< Label for option B button
    EventConsequence choiceA  = {};
    EventConsequence choiceB  = {};
};

namespace EventRegistry
{
    inline constexpr std::array<EventData, 7> k_events =
    {{
        {
            0, "Mice Infestation",
            "A health inspector just spotted mice in the kitchen!",
            "Shut kitchen briefly (-$30, safe)",
            "Bribe the inspector (-$60, stays open)",
            { -30,  -5, 0.8f, 1.0f, "Kitchen closed briefly. Customers notice." },
            { -60,   5, 1.0f, 1.0f, "Inspector paid off. Business as usual." },
        },
        {
            1, "Food Critic Visit",
            "A well-known food critic just walked in. They're watching everything.",
            "Comp their meal (+rep, -money)",
            "Treat them like any customer (risky)",
            {  -20,  20, 1.2f, 1.0f, "Great review incoming! Reputation surges." },
            {    0,   0, 1.0f, 0.9f, "They didn't look impressed. No change." },
        },
        {
            2, "Ingredient Shortage",
            "Your main supplier just called — only half your ingredients arrived.",
            "Limit the menu (-money/customer)",
            "Raise prices to compensate (+money/customer, -patience)",
            {    0,  -5, 1.0f, 1.0f, "Smaller menu, customers a bit disappointed." },
            {   20, -10, 1.0f, 0.8f, "Higher prices anger some customers." },
        },
        {
            3, "Online Drama",
            "Someone posted a viral negative review. Comments are blowing up.",
            "Respond publicly with a coupon (-$40, +rep)",
            "Ignore it and stay focused (reputation bleeds)",
            {  -40,  15, 1.1f, 1.0f, "Coupon offer praised. New customers show up!" },
            {    0, -15, 1.0f, 1.0f, "Drama fades but reputation takes a hit." },
        },
        {
            4, "Viral Trend Opportunity",
            "A popular food challenge is trending. Join it now?",
            "Participate! (+spawn, +money)",
            "Stay classy, don't chase trends",
            {   20,  10, 1.5f, 1.0f, "Video goes viral. Rush of new customers!" },
            {    0,   5, 1.0f, 1.0f, "Reputation steady. Regulars appreciate it." },
        },
        {
            5, "Staff Called In Sick",
            "Your main staff member is sick and running late today.",
            "Call a temp worker (-$25, normal service)",
            "Push through alone (slower service, no cost)",
            {  -25,   0, 1.0f, 1.0f, "Temp hired. Service continues normally." },
            {    0,  -5, 0.7f, 1.0f, "Short-staffed. Customers wait longer." },
        },
        {
            6, "Local Festival Nearby",
            "A street festival opened two blocks away — huge foot traffic!",
            "Hand out flyers (-$15, huge spawn boost)",
            "Let word of mouth do the work (+small spawn)",
            {  -15,   5, 2.0f, 1.0f, "Flyers work! Double the crowd today." },
            {    0,   2, 1.3f, 1.0f, "Some festival-goers wander in naturally." },
        },
    }};

    inline const EventData* Find(int id)
    {
        for (const auto& e : k_events)
            if (e.id == id) return &e;
        return nullptr;
    }
}
