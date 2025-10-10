#include "Scene_World.h"
#include "imgui.h"
#include "Game.h"

void WorldScene::Initialize()
{
	m_resources = std::make_unique<ResourcesManager>();
	m_resources = std::make_unique<ResourcesManager>();
	Components::Texture pirateTexture = m_game.lock()->GetRenderSystem()->LoadTexture("assets//textures//link.bmp");
	Components::SpriteSheet pirateSpriteSheet;
	pirateSpriteSheet.texture = pirateTexture;
	pirateSpriteSheet.frames = {
		{0, 0, 846, 702},
		{32, 0, 32, 32},
		{64, 0, 32, 32},
		{96, 0, 32, 32},
	};
	m_resources->StoreSpriteSheet(1, pirateSpriteSheet);

	entt::entity entity = m_Registry.create();
	m_Registry.emplace<Components::Transform2D>(entity, Components::Transform2D{ {100.0f,100.0f}, {1.0f,1.0f}, 0.0f });
	Components::Sprite pirateSprite(pirateTexture);
	pirateSprite.source = pirateSpriteSheet.frames[0];
	m_Registry.emplace<Components::Sprite>(entity, Components::Sprite{ pirateSprite });
}

void WorldScene::Update(const double& deltaTime)
{
}

void WorldScene::FixedUpdate(const double& deltaTime)
{
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
