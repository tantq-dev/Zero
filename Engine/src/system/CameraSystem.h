#pragma once  
#include "core/Components.h"  
#include "utilities/Vec2.h"
#include <unordered_map>
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
	private:
		std::unordered_map < std::string, Components::Camera> cameraMap;
		std::string m_currentCamera = std::string();
	};


}