#pragma once
#include "Scene.h"
#include <memory>
#include <string>
#include <vector>

namespace System { class UISystem; }
class Desk;
class Seat;

class GameplayScene : public Core::Scene
{
    void Initialize() override;
    void Update(const double& deltaTime) override;
    void FixedUpdate(const double& deltaTime) override;
    void Render(::IRenderer2D& renderer) override;
    void HandleInput() override;
    void HandleUI(System::UISystem& ui) override;
    void OnSceneUnload() override;

private:
    // ---- Fonts ----
    uint32_t m_font    = 0;
    uint32_t m_fontBig = 0;

    // ---- Scene actors ----
    std::shared_ptr<Core::Actor>            m_backgroundActor;
    std::vector<std::shared_ptr<Seat>>      m_seats;
    std::vector<std::shared_ptr<Desk>>      m_desks;
    std::vector<std::shared_ptr<Core::Actor>> m_FoodCart;
    std::vector<std::shared_ptr<Core::Actor>> m_customers;
    std::vector<std::shared_ptr<Core::Actor>> m_staffs;

    // ---- Customer atlas ----
    uint32_t m_customerAtlasId = 0;

    // ---- In-game clock ----
    double m_clockTime  = 0.0;   ///< 0–1440 (minutes in a day)

    // ---- Day timer (real-time seconds) ----
    float  m_dayTimer         = 0.0f;
    float  m_dayDurationSec   = 120.0f; ///< Baked from GameMode
    bool   m_dayEnded         = false;

    // ---- Simulation params (baked from RestaurantGameMode on init) ----
    float  m_spawnInterval    = 10.0f;
    float  m_spawnTimer       = 0.0f;
    int    m_moneyPerCustomer = 10;
    float  m_customerPatience = 10.0f;
    float  m_cookTimeMult     = 1.0f;
    bool   m_hasDelivery      = false;
    float  m_deliveryIncome   = 0.0f;   ///< $/s from delivery counter
    float  m_deliveryTimer    = 0.0f;

    // ---- Reputation & money (synced to RunState at day end) ----
    int    m_reputation       = 50;
    int    m_currentMoney     = 0;
    int    m_totalServed      = 0;
    int    m_satisfiedCount   = 0;

    // ---- Random event ----
    bool   m_eventPending     = false;
    bool   m_eventTriggered   = false;  ///< Guard: only fire once per day
    int    m_activeEventId    = -1;
    uint32_t m_fontEvent      = 0;
    // Runtime multipliers applied after an event choice
    float  m_eventSpawnMult   = 1.0f;
    float  m_eventPatMult     = 1.0f;

    // ---- Helpers ----
    std::string GetCurrentTimeString(double time) const;
    void DrawHUD(System::UISystem& ui);
    void DrawEventOverlay(System::UISystem& ui);
    void ApplyEventConsequence(int choiceIndex);
    void EndDay();
};
