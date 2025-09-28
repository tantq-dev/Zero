#include "Scene_World.h"
#include "imgui.h"
#include "Game.h"

void WorldScene::Initialize(SDL_Renderer& renderer)
{

}

void WorldScene::Update(const double& deltaTime)
{
}

void WorldScene::FixedUpdate(const double& deltaTime)
{
}

void WorldScene::Render(SDL_Renderer& renderer, const double& alpha)
{
	SDL_SetRenderDrawColorFloat(&renderer, 0, 1, 0, 1);
}

void WorldScene::HandleInput(SDL_Event& event)
{
}

void WorldScene::HandleUI(SDL_Event& event)
{
	ImGui::Begin("World Scene");
	ImGui::Text("Welcome to the World Scene");
	if (ImGui::Button("Back to menu")) {
		if (auto g = m_game.lock())
		{
			g->SetActiveScene("MainScene");
		}
	}

	ImGui::End();

	
}
