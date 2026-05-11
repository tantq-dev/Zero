#pragma once
#include "Scene.h"
namespace System { class UISystem; }


class GameplayScene : public Core::Scene {
	 void Initialize() override;
	 void Update(const double& deltaTime) override;
	 void FixedUpdate(const double& deltaTime) override;
	 void Render(::IRenderer2D& renderer) override;
	 void HandleInput() override;
	 void HandleUI(System::UISystem& ui) override;
private:
	 uint32_t m_Font = 0;
	 double m_current = 0;
	 std::shared_ptr<Core::Actor> m_backgroundActor;
	 std::string GetCurrentTimeString(double& time);
};