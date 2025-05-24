#include <stdio.h>
#include "core/Game.h"

int main(int, char**) {
   
    Core::Game game;
	game.Initialize();
	game.Run();
    return 0;
}
