#include "Scene_World.h"
#include "imgui.h"
#include "Game.h"
#include <fstream>	



void WorldScene::Initialize()
{

	std::string sceneName = "WorldScene";
	std::string mapFile = "assets//map//map1.txt";
	std::string tilesetFile = "assets//textures//Wall_Tiles.bmp";
	Vec2 tileSize = { 32,32 };
	Vec2 tileSheetSize = { 400,400 };

	// Initialize ResourcesManager
	m_resources = std::make_unique<ResourcesManager>();
	
	// Set ResourcesManager in RenderSystem so it can access spritesheets
	if (auto game = m_game.lock())
	{
		game->GetRenderSystem()->SetResourcesManager(m_resources.get());
	}
	
	// Load tileset texture and create spritesheet
	Components::Texture tilesetTexture = m_game.lock()->GetRenderSystem()->LoadTexture(tilesetFile);
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

	// ✅ CREATE THE TILE ENTITIES
	m_grid.CreateTileEntities(m_Registry, mapData, 1);
	
	LOG_INFO("Created " + std::to_string(m_grid.GetTileCount()) + " tile entities");
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
	ImGui::Text("Tiles: %d", m_grid.GetTileCount());
	if (ImGui::Button("Back to menu")) {
		if (auto g = m_game.lock())
		{
			g->SetActiveScene("MainScene");
		}
	}

	ImGui::End();

}
