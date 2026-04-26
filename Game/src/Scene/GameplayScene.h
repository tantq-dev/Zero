#pragma once
#include "Scene.h"
struct Tile
{
	Vec2 position;
	bool isWhite;
};

class GameplayScene : public Core::Scene {
	 void Initialize() override;
	 void Update(const double& deltaTime) override;
	 void FixedUpdate(const double& deltaTime) override;
	 void Render(::IRenderer2D& renderer) override;
	 void HandleInput() override;
	 void HandleUI() override;

	 std::vector<Tile> m_tiles;
};