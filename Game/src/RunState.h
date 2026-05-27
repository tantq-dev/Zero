#pragma once
#include <vector>

/// Persistent run data that survives across all scene switches.
/// Lives inside DreamyGameInstance — never in engine code.
struct RunState
{
    int currentDay           = 1;
    int money                = 0;
    int reputation           = 50;   // 0–100
    int totalCustomersServed = 0;
    int satisfiedCustomers   = 0;

    // Indices into UpgradeRegistry::All()
    std::vector<int> activeUpgrades;
    // Indices into BuffRegistry::All()
    std::vector<int> activeBuffs;

    bool HasUpgrade(int id) const
    {
        for (int u : activeUpgrades) if (u == id) return true;
        return false;
    }

    bool HasBuff(int id) const
    {
        for (int b : activeBuffs) if (b == id) return true;
        return false;
    }

    void Reset() { *this = RunState{}; }
};
