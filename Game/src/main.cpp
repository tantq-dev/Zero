#include <iostream>
#include <string>
#include "Scene/GameplayScene.h"
#include "Scene/PreparationScene.h"
#include "Scene/ResultScene.h"
#include "DreamyGameInstance.h"
#include "Game.h"

int main()
{
    auto game = std::make_shared<Core::Game>();
    game->Initialize();

    // Create the persistent game instance — survives all scene switches
    game->CreateGameInstance<DreamyGameInstance>();

    game->AddScene("PreparationScene", std::make_shared<PreparationScene>());
    game->AddScene("GameplayScene",    std::make_shared<GameplayScene>());
    game->AddScene("ResultScene",      std::make_shared<ResultScene>());

    game->SetActiveScene("PreparationScene");
    game->Run();
    return 0;
}
