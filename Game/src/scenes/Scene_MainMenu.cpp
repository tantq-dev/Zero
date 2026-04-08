#include "Scene_MainMenu.h"
#ifdef ZERO_USE_IMGUI
#include "imgui.h"
#endif
#include "Game.h"
#include "AudioComponents.h"


void MainMenuScene::Initialize()  {
	m_resources = std::make_unique<ResourcesManager>();
	
	if (auto game = m_game.lock())
	{
		game->GetRenderSystem().SetResourcesManager(m_resources.get());
		uint32_t srcId = game->GetAudioSystem().LoadWAV("game_assets//audio//test.wav");
		entt::entity background = m_Registry.create();
		m_Registry.emplace<Components::AudioSource>(background,srcId);
		auto& audioSrc = m_Registry.get<Components::AudioSource>(background);
		audioSrc.requestPlay = true;
	}
	
	Components::Texture pirateTexture = m_game.lock()->GetRenderSystem().LoadTexture("engine_assets//textures//pirate.bmp");
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
	Components::AnimationClip testClip{
		.spriteSheetId = 1,
		.frameIndexStart = 0,
		.numberOfFrames = 4,
		.isLoop = true,
		.frameTime = 0.1f
	};
	Components::Animation testAnim(testClip);
	testAnim.currentFrame = 0;

	pirateSprite.source = pirateSpriteSheet.frames[0];
	m_Registry.emplace<Components::Sprite>(entity, Components::Sprite{ pirateSprite });
	m_Registry.emplace<Components::Animation>(entity, testAnim);

	// Add "Menu" Text
	entt::entity menuText = m_Registry.create();
	m_Registry.emplace<Components::Transform2D>(menuText, Components::Transform2D{ {400.0f, 100.0f}, {1.0f, 1.0f}, 0.0f });
	Components::Text textComp("Menu", "engine_assets//fonts//default.ttf", 48.0f);
	textComp.color = { 255, 255, 255, 255 }; // White
	textComp.align = Components::TextAlign::Center;
	m_Registry.emplace<Components::Text>(menuText, textComp);

	Components::InputAction enterLevel("EnterLevel");
	enterLevel.AddBinding(SDL_SCANCODE_W);
	enterLevel.AddBinding(SDL_SCANCODE_UP);
	m_inputSystem.RegisterAction(enterLevel);
}
void MainMenuScene::Update(const double& deltaTime)  {
	// Update logic here
	if (m_inputSystem.IsActionHeld("EnterLevel")) {
		if (auto g = m_game.lock())
		{
			g->SetActiveScene("WorldScene");
		}
	}
}
void MainMenuScene::FixedUpdate(const double& deltaTime)  {
	// Fixed update logic here
}

void MainMenuScene::HandleInput(SDL_Event& event)  {

	m_inputSystem.HandleInput(event);
	// Input handling code here
	
	
}
void MainMenuScene::HandleUI(SDL_Event& event)  {
#ifdef ZERO_USE_IMGUI
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
#endif
}