#include <Game.h>
#include "Scene.h"
#include "scenes/Scene_MainMenu.h"
#include "scenes/Scene_World.h"

int main() {
	std::shared_ptr<Core::Game> game = std::make_shared<Core::Game>();
	game->Initialize();
	game->AddScene("MainScene", std::make_shared<MainMenuScene>());
	game->AddScene("WorldScene", std::make_shared<WorldScene>());
	game->SetActiveScene("MainScene");
	game->Run();

	return 0;
}