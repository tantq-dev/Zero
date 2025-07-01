#include "PathPlacingTool.h"
#include <resources/ModelImporter.h>

void Tool::PathPlacingTool::InitializePathPlacement()
{


}

void Tool::PathPlacingTool::UpdatePathPlacement(float deltaTime)
{
}

void Tool::PathPlacingTool::Initialize(SDL_Renderer& renderer)
{
	m_renderSystem = std::make_unique<System::RenderSystem>();
	m_cameraSystem = std::make_unique<System::CameraSystem>();

	std::shared_ptr<Core::Mesh> mesh;
	Core::ModelLoader loader;
	mesh = loader.LoadModel("assets/models/HS.fbx");

	m_heightMap.LoadHeightMap(mesh, &renderer);
	entt::entity entity = m_registry.create();
	m_registry.emplace<Components::Transform2D>(entity);
	m_registry.emplace<Components::Sprite>(entity, m_heightMap.GetTexturePtr());

	Components::Camera camera;
	camera.SetZoom(100); // default zoom
	m_cameraSystem->AddCamera("Main", camera);
	m_cameraSystem->SetCurrentCamera("Main");

}

void Tool::PathPlacingTool::Update(float deltaTime)
{
}

void Tool::PathPlacingTool::HandleInput(SDL_Event& event)
{

}

void Tool::PathPlacingTool::Render(SDL_Renderer& renderer)
{

	m_renderSystem->RenderSprite(m_registry, renderer, *m_cameraSystem);
}

void Tool::PathPlacingTool::HandleUI(SDL_Event& event)
{
}
