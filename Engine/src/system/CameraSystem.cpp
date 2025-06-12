#include "CameraSystem.h"
namespace System {
	void CameraSystem::SetCameraPosition(const Vec2 pos)
	{
		Components::Camera& currentCam = cameraMap.at(m_currentCamera);
		currentCam.SetPosition(pos);
	}
	Vec2 CameraSystem::GetCameraPosition() const
	{
		return cameraMap.at(m_currentCamera).GetPosition();
	}
	int CameraSystem::GetCameraZoom() const
	{
		return cameraMap.at(m_currentCamera).GetZoom();
	};
	void CameraSystem::AdjustCameraZoom(const float zoom)
	{
		cameraMap.at(m_currentCamera).AdjustZoom(zoom);
	}
	void CameraSystem::AddCamera(const std::string name, const Components::Camera cam)
	{
		cameraMap.insert_or_assign(name, cam);
	}
	const Components::Camera& CameraSystem::GetCurrentCamera()
	{
		return cameraMap.at(m_currentCamera);
	}
	void CameraSystem::AdjustCameraPosition(const Vec2 offset)
	{
		cameraMap.at(m_currentCamera).Adjust(offset);
	}
	void CameraSystem::CameraViewToWorld(const Vec2& viewPos, Vec2& worldPos)
	{
		float zoom = cameraMap.at(m_currentCamera).GetZoom();
		Vec2 camPos = cameraMap.at(m_currentCamera).GetPosition();
		Vec2 screenCenter = m_screenSize * 0.5f;

		worldPos.x = ((viewPos.x - screenCenter.x) / zoom) + camPos.x;
		worldPos.y = ((viewPos.y - screenCenter.y) / zoom) + camPos.y;
	}

	void CameraSystem::WorldToCameraView(const Vec2& worldPos, Vec2& viewPos)
	{
		float zoom = cameraMap.at(m_currentCamera).GetZoom();
		Vec2 camPos = cameraMap.at(m_currentCamera).GetPosition();
		Vec2 screenCenter = m_screenSize * 0.5f;

		viewPos.x = (worldPos.x - camPos.x) * zoom + screenCenter.x;
		viewPos.y = (worldPos.y - camPos.y) * zoom + screenCenter.y;
	}
}

