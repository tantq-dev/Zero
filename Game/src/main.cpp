#include <iostream>
#include <string>
#include "Scene/GameplayScene.h"
#include "Game.h" // GameEngine entry
int main()
{
    
    auto game = std::make_shared<Core::Game>();
    game->Initialize();
    game->AddScene("GameplayScene",  std::make_shared<GameplayScene>());
    game->SetActiveScene("GameplayScene");
    game->Run();
    return 0;
}