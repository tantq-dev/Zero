#include "Scene_MainMenu.h"
#include "imgui.h"
#include "Game.h"

void MainMenuScene::Initialize()  {
	m_resources = std::make_unique<ResourcesManager>();
	
	if (auto game = m_game.lock())
	{
		game->GetRenderSystem()->SetResourcesManager(m_resources.get());
	}
	
	Components::Texture pirateTexture = m_game.lock()->GetRenderSystem()->LoadTexture("assets//textures//pirate.bmp");
	Components::SpriteSheet pirateSpriteSheet;
	pirateSpriteSheet.texture = pirateTexture;
	pirateSpriteSheet.frames = {
		{0, 0, 40, 40},
		{64, 0, 40, 40},
		{128, 0, 40, 40},
		{192, 0, 40, 40},
	};
	m_resources->StoreSpriteSheet(1, pirateSpriteSheet);

	entt::entity entity = m_Registry.create();
	m_Registry.emplace<Components::Transform2D>(entity, Components::Transform2D{ {50.0f,50.0f}, {1.0f,1.0f}, 0.0f });
	Components::Sprite pirateSprite(pirateTexture);
	Components::AnimationClip testClip(1,4,true,0.1);
	Components::Animation testAnim(testClip);
	testAnim.currentFrame = 0;

	pirateSprite.source = pirateSpriteSheet.frames[0];
	m_Registry.emplace<Components::Sprite>(entity, Components::Sprite{ pirateSprite });
	m_Registry.emplace<Components::Animation>(entity, testAnim);


}
void MainMenuScene::Update(const double& deltaTime)  {
	// Update logic here
}
void MainMenuScene::FixedUpdate(const double& deltaTime)  {
	// Fixed update logic here
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