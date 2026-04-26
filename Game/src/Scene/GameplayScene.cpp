#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Player/PlayerFactory.h"
#include "Game.h"




void GameplayScene::Initialize()
{
    auto& reg = GetWorld().Registry;
    auto p = CreatePlayer(reg);
    auto c = reg.create();
    reg.emplace < Components::CameraComponent>(c,Vec2{0,0},320.0f,180.0f);

   
}

void GameplayScene::Update(const double& deltaTime)
{
    auto& reg = GetWorld().Registry;
    auto view = reg.view<PlayerTag, Components::Transform2D>();
    auto player = view.front();
    if (player == entt::null)
    {
        return;
    }
    auto& trans = reg.get<Components::Transform2D>(player);
    trans.position += Vec2{ static_cast<float>(deltaTime*100), 0.0f };
    auto camView = reg.view<Components::CameraComponent>();
    auto cam = camView.front();
    if (cam == entt::null)
    {
        return;
    }
    auto& camProps = reg.get<Components::CameraComponent>(cam);
    camProps.position = trans.position;
    

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
