#include "Scene_MainMenu.h"
#include "imgui.h"
#include "Game.h"

void MainMenuScene::Initialize(SDL_Renderer& renderer)  {
	// Initialization code here
}
void MainMenuScene::Update(const double& deltaTime)  {
	// Update logic here
}
void MainMenuScene::FixedUpdate(const double& deltaTime)  {
	// Fixed update logic here
}
void MainMenuScene::Render(SDL_Renderer& renderer, const double& alpha)  {
	SDL_SetRenderDrawColorFloat(&renderer, 1,0,0,1);
}
void MainMenuScene::HandleInput(SDL_Event& event)  {
	// Input handling code here
}
void MainMenuScene::HandleUI(SDL_Event& event)  {
	ImGui::Begin("Main Menu");
	ImGui::Text("Welcome to the Main Menu");
	if (ImGui::Button("Enter world"))
	{
		if (auto g = m_game.lock())
		{
			g->SetActiveScene("WorldScene");
		}
	
	}
	ImGui::End();
}