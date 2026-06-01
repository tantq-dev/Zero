#pragma once
#include "Scene.h"
#include <string>

class ResultScene : public Core::Scene
{
public:
    void Initialize() override;
    void Update(const double& deltaTime) override;
    void FixedUpdate(const double& deltaTime) override;
    void Render(::IRenderer2D& renderer) override;
    void HandleInput() override;
    void HandleUI(System::UISystem& ui) override;
    void OnSceneUnload() override;


private:
    uint32_t m_fontTitle  = 0;
    uint32_t m_fontBody   = 0;

    std::string GetArchetypeLabel(int money, int reputation, bool hasDelivery) const;
};
