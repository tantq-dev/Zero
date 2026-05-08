#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Player/PlayerFactory.h"
#include "Player/Player.h"
#include "Game.h"




void GameplayScene::Initialize()
{
    auto& reg = GetWorld()->Registry;
    auto cam = reg.create();
    reg.emplace < Components::CameraComponent>(cam,Vec2{0,0},320.0f,180.0f);
    auto player = m_world->SpawnActor<Player>();

    m_actors.push_back(player);

   
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

void GameplayScene::HandleUI()
{
}
