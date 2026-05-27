#include "Scene/PreparationScene.h"
#include "UISystem.h"
#include "Game.h"
#include "DreamyGameInstance.h"
#include "UpgradeRegistry.h"
#include "BuffRegistry.h"
#include <cstdlib>
#include <ctime>

// ---- Layout constants -------------------------------------------------------
namespace {
    constexpr float kScreenW    = 1280.0f/2;
    constexpr float kScreenH    = 720.0f/2;
    constexpr float kCardW      = 280.0f/2;
    constexpr float kCardH      = 340.0f/2;
    constexpr float kCardGap    = 40.0f/2;
    constexpr float kCardY      = 180.0f/2;

    // 3 cards centred horizontally
    constexpr float kTotalCardsW = kCardW * 3 + kCardGap * 2;
    constexpr float kCardsStartX = (kScreenW - kTotalCardsW) * 0.5f;
}

PreparationScene::PreparationScene() {}

void PreparationScene::Initialize()
{
    auto game = m_game.lock();
    if (!game) return;

    auto& renderer = game->GetRenderSystem().GetRenderer();
    m_font    = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 18, renderer);
    m_fontBig = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 28, renderer);

    // Seed random draw from current day so each day offers different cards
    auto& gi = game->GetGameInstance<DreamyGameInstance>();
    std::srand(static_cast<unsigned>(std::time(nullptr)) + gi.runState.currentDay * 137);

    // Draw 3 unique upgrade offers
    m_offeredUpgrades.clear();
    m_selectedUpgrade = -1;
    {
        const int total = static_cast<int>(UpgradeRegistry::k_upgrades.size());
        std::vector<int> pool;
        for (int i = 0; i < total; ++i) pool.push_back(i);
        // shuffle first 3
        for (int i = 0; i < 3; ++i) {
            int j = i + std::rand() % (total - i);
            std::swap(pool[i], pool[j]);
        }
        for (int i = 0; i < 3; ++i) m_offeredUpgrades.push_back(pool[i]);
    }

    // Draw 3 unique buff offers
    m_offeredBuffs.clear();
    m_selectedBuff = -1;
    {
        const int total = static_cast<int>(BuffRegistry::k_buffs.size());
        std::vector<int> pool;
        for (int i = 0; i < total; ++i) pool.push_back(i);
        for (int i = 0; i < 3; ++i) {
            int j = i + std::rand() % (total - i);
            std::swap(pool[i], pool[j]);
        }
        for (int i = 0; i < 3; ++i) m_offeredBuffs.push_back(pool[i]);
    }

    m_currentState = EPreparationState::Upgrade;
}

void PreparationScene::Update(const double& dt)  {}
void PreparationScene::FixedUpdate(const double& dt) {}
void PreparationScene::Render(::IRenderer2D& renderer) {}
void PreparationScene::HandleInput() {}

// ---- UI ---------------------------------------------------------------------

void PreparationScene::HandleUI(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;

    auto& gi = game->GetGameInstance<DreamyGameInstance>();

    // Dark semi-transparent background panel
    ui.Panel({ 0, 0, static_cast<int>(kScreenW), static_cast<int>(kScreenH) }, { 18, 15, 35, 255 });

    // Day header
    std::string dayHeader = "Day " + std::to_string(gi.runState.currentDay) + " of 7 — Choose Wisely";
    ui.Label(dayHeader, kScreenW * 0.5f, 30.0f, m_fontBig, { 255, 220, 100, 255 }, Components::TextAlign::Center);

    if (m_currentState == EPreparationState::Upgrade)
        DrawUpgradePhase(ui);
    else
        DrawBuffPhase(ui);
}

void PreparationScene::DrawUpgradePhase(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;

    ui.Label("Physical Upgrade", kScreenW * 0.5f, 80.0f, m_fontBig, { 180, 220, 255, 255 }, Components::TextAlign::Center);
    ui.Label("Expand your restaurant — choose one improvement.", kScreenW * 0.5f, 118.0f, m_font, { 180, 180, 200, 200 }, Components::TextAlign::Center);

    for (int slot = 0; slot < 3; ++slot)
    {
        int uid = m_offeredUpgrades[slot];
        const UpgradeData* u = UpgradeRegistry::Find(uid);
        if (!u) continue;

        float x = kCardsStartX + slot * (kCardW + kCardGap);
        bool selected = (m_selectedUpgrade == slot);

        if (CardButton(ui, x, kCardY, kCardW, kCardH, u->name, u->description, selected))
            m_selectedUpgrade = slot;
    }

    // Confirm button — only active when a card is chosen
    if (m_selectedUpgrade >= 0)
    {
        float btnW = 220.0f, btnH = 50.0f;
        float btnX = (kScreenW - btnW) * 0.5f;
        float btnY = kCardY + kCardH + 30.0f;

        ui.Panel({ btnX, btnY, btnW, btnH }, { 80, 200, 120, 255 });
        ui.Label("Confirm Upgrade", btnX + btnW * 0.5f, btnY + 14.0f, m_font, { 10, 10, 10, 255 }, Components::TextAlign::Center);

        if (ui.Button("Confirm Upgrade",
            { btnX, btnY, btnW, btnH },
            m_font))
        {
            auto game2 = m_game.lock();
            if (game2)
            {
                auto& gi = game2->GetGameInstance<DreamyGameInstance>();
                gi.runState.activeUpgrades.push_back(m_offeredUpgrades[m_selectedUpgrade]);
            }
            m_currentState = EPreparationState::Buff;
        }
    }
}

void PreparationScene::DrawBuffPhase(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;

    ui.Label("Business Buff", kScreenW * 0.5f, 80.0f, m_fontBig, { 255, 180, 120, 255 }, Components::TextAlign::Center);
    ui.Label("Choose a buff that changes how customers behave today.", kScreenW * 0.5f, 118.0f, m_font, { 180, 180, 200, 200 }, Components::TextAlign::Center);

    for (int slot = 0; slot < 3; ++slot)
    {
        int bid = m_offeredBuffs[slot];
        const BuffData* b = BuffRegistry::Find(bid);
        if (!b) continue;

        float x = kCardsStartX + slot * (kCardW + kCardGap);
        bool selected = (m_selectedBuff == slot);

        if (CardButton(ui, x, kCardY, kCardW, kCardH, b->name, b->description, selected))
            m_selectedBuff = slot;
    }

    // Confirm button
    if (m_selectedBuff >= 0)
    {
        float btnW = 220.0f, btnH = 50.0f;
        float btnX = (kScreenW - btnW) * 0.5f;
        float btnY = kCardY + kCardH + 30.0f;

        ui.Panel({ btnX, btnY, btnW, btnH }, { 200, 120, 80, 255 });
        ui.Label("Open Restaurant!", btnX + btnW * 0.5f, btnY + 14.0f, m_font, { 10, 10, 10, 255 }, Components::TextAlign::Center);

        if (ui.Button("Open Restaurant!",
            { btnX, btnY, btnW, btnH },
            m_font))
        {
            auto game2 = m_game.lock();
            if (game2)
            {
                auto& gi = game2->GetGameInstance<DreamyGameInstance>();
                gi.runState.activeBuffs.push_back(m_offeredBuffs[m_selectedBuff]);
                game2->SetActiveScene("GameplayScene");
            }
        }
    }
}

bool PreparationScene::CardButton(System::UISystem& ui,
    float x, float y, float w, float h,
    const char* title, const char* desc,
    bool selected)
{
    Components::Color bgColor  = selected ? Components::Color{60, 80, 120, 255}
                                           : Components::Color{30, 28, 55, 230};
    Components::Color border   = selected ? Components::Color{100, 180, 255, 255}
                                           : Components::Color{70, 60, 100, 200};

    // Card background
    ui.Panel({ static_cast<float>(x),     static_cast<float>(y),
               static_cast<float>(w),     static_cast<float>(h) }, bgColor);
    // Card border (4px inner shadow effect via a thinner panel)
    ui.Panel({ static_cast<float>(x)+2,   static_cast<float>(y)+2,
               static_cast<float>(w)-4,   static_cast<float>(h)-4 }, border);
    ui.Panel({ static_cast<float>(x)+4,   static_cast<float>(y)+4,
               static_cast<float>(w)-8,   static_cast<float>(h)-8 }, bgColor);

    // Title
    ui.Label(title, x + w * 0.5f, y + 20.0f, m_fontBig, { 255, 235, 180, 255 }, Components::TextAlign::Center);

    // Description — word-wrap approximated by label
    ui.Label(desc, x + 18.0f, y + 80.0f, m_font, { 200, 200, 220, 220 }, Components::TextAlign::Left);

    // Selected indicator
    if (selected)
        ui.Label("✓ Selected", x + w * 0.5f, y + h - 36.0f, m_font, { 100, 255, 140, 255 }, Components::TextAlign::Center);

    // Invisible hit-test button on top
    return ui.Button("",
        { static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) },
        m_font);
}
