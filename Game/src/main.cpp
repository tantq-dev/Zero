#include <stdio.h>
#include "core/Game.h"
#include <Tool.h>


int main(int, char**) {

	// Initialize default scenes
	Core::Game game;
	auto tool = std::make_shared<Tool::Tool>();
	tool->SetGame(std::shared_ptr<Core::Game>(&game, [](Core::Game*) {})); // Non-owning shared_ptr
	game.AddScene("tool", tool);
	game.SetActiveScene("tool");
	game.Initialize();
	game.Run();



	return 0;
}