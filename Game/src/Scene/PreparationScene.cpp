#include "Scene/PreparationScene.h"
#include "UISystem.h"
#include "Game.h"
#include "DreamyGameInstance.h"
#include "GameplayAbility/Ability.h"
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
        std::vector<int> available;
        for (const auto& upgrade : UpgradeRegistry::All()) {
            if (!gi.runState.HasUpgrade(upgrade.Id)) {
                available.push_back(upgrade.Id);
            }
        }

        while (!available.empty() && m_offeredUpgrades.size() < 3) {
            const int index = std::rand() % static_cast<int>(available.size());
            m_offeredUpgrades.push_back(available[index]);
            available.erase(available.begin() + index);
        }
    }

    // Draw 3 unique buff offers
    m_offeredBuffs.clear();
    m_selectedBuff = -1;
    {
        std::vector<int> available;
        for (const auto& buff : BuffRegistry::All()) {
            available.push_back(buff.Id);
        }

        while (!available.empty() && m_offeredBuffs.size() < 3) {
            const int index = std::rand() % static_cast<int>(available.size());
            m_offeredBuffs.push_back(available[index]);
            available.erase(available.begin() + index);
        }
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

void PreparationScene::OnSceneUnload()
{
    m_offeredUpgrades.clear();
    m_offeredBuffs.clear();
    m_selectedUpgrade = -1;
    m_selectedBuff = -1;
    m_currentState = EPreparationState::Upgrade;
    m_font = 0;
    m_fontBig = 0;

    if (m_world) {
        m_world->Clear();
    }
}

void PreparationScene::DrawUpgradePhase(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;

    ui.Label("Physical Upgrade", kScreenW * 0.5f, 80.0f, m_fontBig, { 180, 220, 255, 255 }, Components::TextAlign::Center);
    ui.Label("Expand your restaurant — choose one improvement.", kScreenW * 0.5f, 118.0f, m_font, { 180, 180, 200, 200 }, Components::TextAlign::Center);

    for (int slot = 0; slot < 3; ++slot)
    {
        if (slot >= static_cast<int>(m_offeredUpgrades.size())) break;

        const UpgradeDefinition* upgrade = UpgradeRegistry::Find(m_offeredUpgrades[slot]);
        if (!upgrade) continue;

        const float x = kCardsStartX + slot * (kCardW + kCardGap);
        const bool selected = (m_selectedUpgrade == slot);
        if (CardButton(ui, x, kCardY, kCardW, kCardH, upgrade->Name, upgrade->Description, selected)) {
            m_selectedUpgrade = slot;
        }
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
            auto& gi = game->GetGameInstance<DreamyGameInstance>();
            gi.runState.activeUpgrades.push_back(m_offeredUpgrades[m_selectedUpgrade]);
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
        if (slot >= static_cast<int>(m_offeredBuffs.size())) break;

        const BuffDefinition* buff = BuffRegistry::Find(m_offeredBuffs[slot]);
        if (!buff) continue;

        const float x = kCardsStartX + slot * (kCardW + kCardGap);
        const bool selected = (m_selectedBuff == slot);
        if (CardButton(ui, x, kCardY, kCardW, kCardH, buff->Name, buff->Description, selected)) {
            m_selectedBuff = slot;
        }
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

void PreparationScene::DrawCard(System::UISystem& ui, float x, float y, float w, float h, const char* title, const char* desc, bool selected, int cardIndex)
{
    (void)cardIndex;
    CardButton(ui, x, y, w, h, title, desc, selected);
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

    // Transparent hit-test button on top.
    UI::UIStyle transparentStyle{};
    transparentStyle.buttonNormal = { 0, 0, 0, 0 };
    transparentStyle.buttonHover = { 255, 255, 255, 18 };
    transparentStyle.buttonPressed = { 100, 180, 255, 35 };
    transparentStyle.textColor = { 0, 0, 0, 0 };

    return ui.Button("",
        { static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) },
        m_font,
        transparentStyle);
}
