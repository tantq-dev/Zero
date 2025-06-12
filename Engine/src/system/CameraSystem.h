#pragma once  
#include "core/Components.h"  
#include "utilities/Vec2.h"
#include <unordered_map>
#include "config/ApplicationConfig.h"
namespace System {
	class CameraSystem
	{
	public:
		CameraSystem() = default;
		~CameraSystem() = default;
		void SetCameraPosition(const Vec2 pos);
		Vec2 GetCameraPosition() const;
		int GetCameraZoom() const;
		void SetCurrentCamera(const std::string c) { m_currentCamera = c; }
		void AdjustCameraZoom(const float zoom);
		void AddCamera(const std::string name, const Components::Camera cam);
		const Components::Camera& GetCurrentCamera();
		void AdjustCameraPosition(const Vec2 offset);
		void CameraViewToWorld(const Vec2& viewPos, Vec2& worldPos);
		void WorldToCameraView(const Vec2& worldPos, Vec2& viewPos);
		void SetScreenSize(const Vec2& screenSize) { m_screenSize = screenSize; }
		Vec2 GetScreenSize() const { return m_screenSize; }
	private:
		std::unordered_map < std::string, Components::Camera> cameraMap;
		std::string m_currentCamera = std::string();
		Vec2 m_screenSize = Vec2(ApplicationConfig::DEFAULT_WINDOW_WIDTH, ApplicationConfig::DEFAULT_WINDOW_HEIGHT); // Default size
	};


}