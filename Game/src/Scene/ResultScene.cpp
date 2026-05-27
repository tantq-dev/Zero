#include "Scene/ResultScene.h"
#include "UISystem.h"
#include "Game.h"
#include "DreamyGameInstance.h"
#include "UpgradeRegistry.h"
#include "BuffRegistry.h"

namespace {
    constexpr float kScreenW = 1280.0f;
    constexpr float kScreenH = 720.0f;
}

void ResultScene::Initialize()
{
    auto game = m_game.lock();
    if (!game) return;
    auto& renderer = game->GetRenderSystem().GetRenderer();
    m_fontTitle = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 36, renderer);
    m_fontBody  = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 22, renderer);
}

void ResultScene::Update(const double&) {}
void ResultScene::FixedUpdate(const double&) {}
void ResultScene::Render(::IRenderer2D&) {}
void ResultScene::HandleInput() {}

void ResultScene::HandleUI(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;
    auto& gi = game->GetGameInstance<DreamyGameInstance>();
    const RunState& rs = gi.runState;

    // Background
    ui.Panel({ 0, 0, static_cast<int>(kScreenW), static_cast<int>(kScreenH) }, { 12, 10, 25, 255 });

    // Title
    ui.Label("Run Complete!", kScreenW * 0.5f, 40.0f, m_fontTitle, { 255, 215, 80, 255 }, Components::TextAlign::Center);

    // Archetype label
    bool hasDelivery = false;
    for (int uid : rs.activeUpgrades) {
        const UpgradeData* u = UpgradeRegistry::Find(uid);
        if (u && u->effect == UpgradeEffect::DeliveryCounter) { hasDelivery = true; break; }
    }
    std::string archetype = GetArchetypeLabel(rs.money, rs.reputation, hasDelivery);
    ui.Label(archetype, kScreenW * 0.5f, 100.0f, m_fontBody, { 200, 170, 255, 255 }, Components::TextAlign::Center);

    // Stats panel
    float panelX = kScreenW * 0.5f - 260.0f;
    float panelY = 160.0f;
    ui.Panel({ static_cast<float>(panelX), static_cast<float>(panelY), 520, 310 }, { 28, 24, 50, 240 });

    float labelX = panelX + 30.0f;
    float valX   = panelX + 490.0f;
    float lineH  = 52.0f;
    float row    = panelY + 25.0f;

    auto statRow = [&](const std::string& label, const std::string& val, Components::Color col) {
        ui.Label(label, labelX, row, m_fontBody, { 180, 180, 210, 255 }, Components::TextAlign::Left);
        ui.Label(val,   valX,  row, m_fontBody, col, Components::TextAlign::Right);
        row += lineH;
    };

    statRow("Total Earnings",         "$" + std::to_string(rs.money),              { 100, 255, 120, 255 });
    statRow("Final Reputation",       std::to_string(rs.reputation) + " / 100",   { 100, 200, 255, 255 });
    statRow("Customers Served",       std::to_string(rs.totalCustomersServed),     { 255, 200, 100, 255 });
    statRow("Customer Satisfaction",  std::to_string(rs.satisfiedCustomers) + " happy", { 200, 150, 255, 255 });
    statRow("Days Survived",          std::to_string(rs.currentDay - 1) + " / 7", { 255, 150, 150, 255 });

    // Active upgrades summary
    float upgradeY = panelY + 330.0f;
    ui.Label("Upgrades Built:", labelX, upgradeY, m_fontBody, { 160, 220, 160, 255 }, Components::TextAlign::Left);
    float ux = labelX + 170.0f;
    for (int uid : rs.activeUpgrades) {
        const UpgradeData* u = UpgradeRegistry::Find(uid);
        if (u) { ui.Label(u->name, ux, upgradeY, m_fontBody, { 200, 240, 200, 200 }, Components::TextAlign::Left); ux += 180.0f; }
    }

    // Play Again button
    float btnW = 240.0f, btnH = 56.0f;
    float btnX = kScreenW * 0.5f - btnW * 0.5f;
    float btnY = kScreenH - 110.0f;

    ui.Panel({ static_cast<float>(btnX), static_cast<float>(btnY), static_cast<float>(btnW), static_cast<float>(btnH) }, { 80, 160, 240, 255 });
    ui.Label("Play Again", btnX + btnW * 0.5f, btnY + 14.0f, m_fontBody, { 255, 255, 255, 255 }, Components::TextAlign::Center);

    if (ui.Button("Play Again",
        { static_cast<float>(btnX), static_cast<float>(btnY), static_cast<float>(btnW), static_cast<float>(btnH) },
        m_fontBody))
    {
        auto game2 = m_game.lock();
        if (game2) {
            game2->GetGameInstance<DreamyGameInstance>().runState.Reset();
            game2->SetActiveScene("PreparationScene");
        }
    }
}

std::string ResultScene::GetArchetypeLabel(int money, int reputation, bool hasDelivery) const
{
    if (hasDelivery && money > 300)
        return "★ Delivery Kitchen — The Passive Income Empire ★";
    if (reputation >= 80)
        return "★ Luxury Café — A Boutique Dream ★";
    if (money > 500)
        return "★ Fast Food Chain — Volume is King ★";
    if (reputation >= 60 && money > 200)
        return "★ Viral Trend Restaurant — Hot Today, Hotter Tomorrow ★";
    return "★ Humble Street Cart — A Journey Begins ★";
}
