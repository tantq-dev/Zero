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
		[[nodiscard]] Vec2 GetCameraPosition() const
		{
			return cameraMap.at(m_currentCamera).GetPosition(); 
		};
		[[nodiscard]] int GetCameraZoom() const
		{
			return cameraMap.at(m_currentCamera).GetZoom(); 
		};	
		void SetCurrentCamera(const std::string c) {
			m_currentCamera = c;
		}

		void AdjustCameraZoom(const float zoom) {
			
			cameraMap.at(m_currentCamera).AdjustZoom(zoom);
		}

		void AddCamera(const std::string name, const Components::Camera cam) {
			cameraMap.insert_or_assign(name, cam);
		}

		const Components::Camera& GetCurrentCamera() {
			return cameraMap.at(m_currentCamera);
		}

		void AdjustCameraPosition(const Vec2 offset) {
			cameraMap.at(m_currentCamera).Adjust(offset);
		}

	private:
		std::unordered_map < std::string, Components::Camera> cameraMap;
		std::string m_currentCamera = std::string();
	};


}