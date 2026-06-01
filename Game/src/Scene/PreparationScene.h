#pragma once
#include "Scene.h"
#include <vector>

class PreparationScene : public Core::Scene
{
public:
    PreparationScene();

    enum class EPreparationState { Upgrade, Buff };

    void Initialize() override;
    void Update(const double& deltaTime) override;
    void FixedUpdate(const double& deltaTime) override;
    void Render(::IRenderer2D& renderer) override;
    void HandleInput() override;
    void HandleUI(System::UISystem& ui) override;
    void OnSceneUnload() override;


private:
    EPreparationState m_currentState = EPreparationState::Upgrade;

    // 3 random offers drawn from the registries each day
    std::vector<int> m_offeredUpgrades;
    std::vector<int> m_offeredBuffs;

    int m_selectedUpgrade = -1; // index into m_offeredUpgrades
    int m_selectedBuff    = -1;

    uint32_t m_font     = 0;
    uint32_t m_fontBig  = 0;

    void DrawUpgradePhase(System::UISystem& ui);
    void DrawBuffPhase(System::UISystem& ui);
    void DrawCard(System::UISystem& ui,
                  float x, float y, float w, float h,
                  const char* title, const char* desc,
                  bool selected, int cardIndex);
    /// Returns true if the card at slot was clicked
    bool CardButton(System::UISystem& ui,
                    float x, float y, float w, float h,
                    const char* title, const char* desc,
                    bool selected);
};
