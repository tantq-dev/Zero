#include <Game.h>
#include "Scene.h"
#include "scenes/Scene_MainMenu.h"
#include "scenes/Scene_World.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

std::shared_ptr<Core::Game> g_game;

#ifdef __EMSCRIPTEN__
void main_loop() {
    if (g_game)
        g_game->Tick();
}
#endif

int main() {
    g_game = std::make_shared<Core::Game>();
    g_game->Initialize();
    g_game->AddScene("MainScene", std::make_shared<MainMenuScene>());
    g_game->AddScene("WorldScene", std::make_shared<Game::WorldScene>());
    g_game->SetActiveScene("MainScene");

#ifdef __EMSCRIPTEN__
    // Use Emscripten main loop instead of blocking loop
    g_game->StartLoop();
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    g_game->Run();  // desktop: blocking loop is fine
#endif

    return 0;
}