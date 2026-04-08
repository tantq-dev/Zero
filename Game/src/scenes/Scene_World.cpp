#include "Scene_World.h"
#ifdef ZERO_USE_IMGUI
#include "imgui.h"
#endif
#include "Game.h"
#include <fstream>	
#include "player/Player.h"
#include "../systems/EnemySystem.h"
#include "../systems/CombatSystem.h"
#include "../core/GameComponents.h"

using namespace Game;

void WorldScene::Initialize()
{

	std::string sceneName = "WorldScene";
	std::string mapFile = "game_assets//map//map1.txt";
	std::string tilesetFile = "game_assets//textures//Wall_Tiles.bmp";
	Vec2 tileSize = { 32,32 };
	Vec2 tileSheetSize = { 400,400 };

	// Initialize ResourcesManager
	m_resources = std::make_unique<ResourcesManager>();
	
	// Set ResourcesManager in RenderSystem so it can access spritesheets
	if (auto game = m_game.lock())
	{
		game->GetRenderSystem().SetResourcesManager(m_resources.get());
	}
	
	// Load tileset texture and create spritesheet
	Components::Texture tilesetTexture = m_game.lock()->GetRenderSystem().LoadTexture(tilesetFile);
	Components::SpriteSheet tileSpriteSheet;
	tileSpriteSheet.texture = tilesetTexture;


	// FIXED: Iterate rows first (j), then columns (i) for correct frame order
	for (size_t j = 0; j < tileSheetSize.y; j += tileSize.y)
	{
		for (size_t i = 0; i < tileSheetSize.x; i += tileSize.x)
		{
			tileSpriteSheet.frames.push_back({ 
				static_cast<float>(i), 
				static_cast<float>(j), 
				static_cast<float>(tileSize.x), 
				static_cast<float>(tileSize.y) 
			});
		}
	}

	m_resources->StoreSpriteSheet(1, tileSpriteSheet);


	Components::Texture runSpriteSheet = m_game.lock()->GetRenderSystem().LoadTexture("game_assets/textures/Sword_Run_full.bmp");
	Components::SpriteSheet runSheet;
	runSheet.texture = runSpriteSheet;
	// Assume each frame is 64x64 and the sheet is 512x256 (8 columns, 4 rows)
	for (size_t j = 0; j < 256; j += 64)
	{
		for (size_t i = 0; i < 384; i += 64)
		{
			runSheet.frames.push_back({
				static_cast<float>(i),
				static_cast<float>(j),
				64.0f,
				64.0f
				});
		}
	}

	m_resources->StoreSpriteSheet(2, runSheet);

	//load map from file and create entities with Tile component
	std::vector<std::vector<int>> mapData;
	std::ifstream mapFileStream(mapFile);
	if (mapFileStream.is_open())
	{
		std::string line;
		while (std::getline(mapFileStream, line))
		{
			std::vector<int> row;
			for (char& ch : line)
			{
				if (ch >= '0' && ch <= '9')
				{
					row.push_back(ch - '0');
				}
			}
			if (!row.empty())
			{
				mapData.push_back(row);
			}
		}
		mapFileStream.close();
	}
	else
	{
		LOG_ERROR("Failed to open map file: " + mapFile);
		return;
	}

	// Initialize grid with actual map dimensions
	int gridWidth = mapData.empty() ? 0 : static_cast<int>(mapData[0].size());
	int gridHeight = static_cast<int>(mapData.size());
	m_grid.Initialize(gridWidth, gridHeight, static_cast<int>(tileSize.x));

	m_grid.CreateTileEntities(m_Registry, mapData, 1);
	
	LOG_INFO("Created " + std::to_string(m_grid.GetTileCount()) + " tile entities");
	
	// Create camera
	CreateCamera();
	
	// Create player
	CreatePlayer();

	// Create Enemies
	CreateEnemy({ 200.0f, 200.0f });
	CreateEnemy({ 600.0f, 400.0f });
	
	// Register input actions
	Components::InputAction moveUp("MoveUp");
	moveUp.AddBinding(SDL_SCANCODE_W);
	moveUp.AddBinding(SDL_SCANCODE_UP);
	m_inputSystem.RegisterAction(moveUp);
	
	Components::InputAction moveDown("MoveDown");
	moveDown.AddBinding(SDL_SCANCODE_S);
	moveDown.AddBinding(SDL_SCANCODE_DOWN);
	m_inputSystem.RegisterAction(moveDown);
	
	Components::InputAction moveLeft("MoveLeft");
	moveLeft.AddBinding(SDL_SCANCODE_A);
	moveLeft.AddBinding(SDL_SCANCODE_LEFT);
	m_inputSystem.RegisterAction(moveLeft);
	
	Components::InputAction moveRight("MoveRight");
	moveRight.AddBinding(SDL_SCANCODE_D);
	moveRight.AddBinding(SDL_SCANCODE_RIGHT);
	m_inputSystem.RegisterAction(moveRight);

	Components::InputAction attack("Attack");
	attack.AddBinding(SDL_SCANCODE_SPACE);
	m_inputSystem.RegisterAction(attack);
}

void WorldScene::CreateCamera()
{
	m_cameraEntity = m_Registry.create();
	
	auto& camera = m_Registry.emplace<Components::Camera>(m_cameraEntity);
	camera.position = { 0.0f, 0.0f };
	camera.zoom = 1.0f;
	camera.width = 800;
	camera.height = 600;
	
	m_cameraSystem.AddCamera("main", camera);
	m_cameraSystem.SetCurrentCamera("main");
	m_cameraSystem.SetScreenSize({ 800.0f, 600.0f });
	
	LOG_INFO("Camera created");
}

void WorldScene::CreatePlayer()
{
	m_playerEntity = m_Registry.create();
	
	// Add Transform component (start position in center of screen)
	auto& transform = m_Registry.emplace<Components::Transform2D>(m_playerEntity);
	transform.position = { 400.0f, 300.0f };
	transform.scale = { 1.0f, 1.0f };
	
	// Add Velocity component
	m_Registry.emplace<Components::Velocity>(m_playerEntity);
	
	// Add Player component
	auto& player = m_Registry.emplace<Character>(m_playerEntity);
	player.moveSpeed = 100.0f;

	// Add MeleeAttack component
	m_Registry.emplace<MeleeAttack>(m_playerEntity);

	// Add Sprite component
	auto& sprite = m_Registry.emplace<Components::Sprite>(m_playerEntity);
	sprite.visible = true;
	sprite.layer = 10; // Above tiles

	// FIXED: Use reference (&) to modify the actual component, not a copy
	auto& directionAnim = m_Registry.emplace<DirectionAnimation>(m_playerEntity);
	
	directionAnim.directionAnimations[CharacterDirection::DOWN].stateAnimations[CharacterState::IDLE] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 0,
		.numberOfFrames = 1,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::DOWN].stateAnimations[CharacterState::MOVING] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 0,
		.numberOfFrames = 6,
		.isLoop = true,
		.frameTime = 0.2f
	};
	
	directionAnim.directionAnimations[CharacterDirection::LEFT].stateAnimations[CharacterState::IDLE] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 6,
		.numberOfFrames = 1,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::LEFT].stateAnimations[CharacterState::MOVING] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 6,
		.numberOfFrames = 6,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::RIGHT].stateAnimations[CharacterState::IDLE] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 12,
		.numberOfFrames = 1,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::RIGHT].stateAnimations[CharacterState::MOVING] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 12,
		.numberOfFrames = 6,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::UP].stateAnimations[CharacterState::IDLE] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 18,
		.numberOfFrames = 1,
		.isLoop = true,
		.frameTime = 0.2f
	};

	directionAnim.directionAnimations[CharacterDirection::UP].stateAnimations[CharacterState::MOVING] =
		Components::AnimationClip{
		.spriteSheetId = 2,
		.frameIndexStart = 18,
		.numberOfFrames = 6,
		.isLoop = true,
		.frameTime = 0.2f
	};

	// FIXED: Use reference (&) here too
	auto& animation = m_Registry.emplace<Components::Animation>(m_playerEntity);
	animation.currentClip = directionAnim.directionAnimations[CharacterDirection::DOWN].stateAnimations[CharacterState::IDLE];
	animation.currentFrame = 0;
	// Load player texture (use a simple colored texture or placeholder)
	// For now, let's try loading an existing texture file or create one
	
	LOG_INFO("Player created at position: " + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y));
}

void WorldScene::CreateEnemy(Vec2 position)
{
	auto entity = m_Registry.create();
	auto& transform = m_Registry.emplace<Components::Transform2D>(entity);
	transform.position = position;
	transform.scale = { 1.0f, 1.0f };

	m_Registry.emplace<Components::Velocity>(entity);
	m_Registry.emplace<Enemy>(entity);
	m_Registry.emplace<Health>(entity);

	// Simple sprite for enemy (reuse player sprite for now, maybe tinted red)
	auto& sprite = m_Registry.emplace<Components::Sprite>(entity);
	sprite.visible = true;
	sprite.layer = 10;
	sprite.spriteSheetId = 2; // Reuse player sheet
	sprite.frameIndex = 0;
	sprite.tint = { 255, 0, 0, 255 }; // Red tint
}

void WorldScene::Update(const double& deltaTime)
{
	// Update movement system
	m_movementSystem.Update(m_Registry, m_inputSystem, static_cast<float>(deltaTime));
	
	// Update Enemy System
	static EnemySystem enemySystem;
	enemySystem.Update(m_Registry, static_cast<float>(deltaTime));

	// Update Combat System
	static CombatSystem combatSystem;
	combatSystem.Update(m_Registry, static_cast<float>(deltaTime));

	// Update animation system
	m_animationSystem.Update(m_Registry, static_cast<float>(deltaTime));
}

void WorldScene::FixedUpdate(const double& deltaTime)
{
}


void WorldScene::HandleInput(SDL_Event& event)
{
	m_inputSystem.HandleInput(event);
}

void WorldScene::HandleUI(SDL_Event& event)
{
#ifdef ZERO_USE_IMGUI
	ImGui::Begin("World Scene");
	ImGui::Text("Welcome to the World Scene");
	ImGui::Text("Tiles: %d", m_grid.GetTileCount());
	if (ImGui::Button("Back to menu")) {
		if (auto g = m_game.lock())
		{
			g->SetActiveScene("MainScene");
		}
	}

	ImGui::End();
#endif

}
