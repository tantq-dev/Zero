#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Player/Player.h"
#include "Game.h"
#include "UISystem.h"




void GameplayScene::Initialize()
{

}

void GameplayScene::Update(const double& deltaTime)
{
 
}

void GameplayScene::FixedUpdate(const double& deltaTime)
{
}

void GameplayScene::Render(::IRenderer2D& renderer)
{
}

void GameplayScene::HandleInput()
{
}

void GameplayScene::HandleUI(System::UISystem& ui)
{
    // Example — remove or replace with real HUD:
    // ui.Panel({0, 0, 320, 20}, {0, 0, 0, 160});
    // ui.Label("Hello UI", 4, 10, myFontId, {255,255,255,255});
}
