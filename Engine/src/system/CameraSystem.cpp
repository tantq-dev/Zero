#include "CameraSystem.h"
namespace System {
	void CameraSystem::SetCameraPosition(const Vec2 pos)
	{
		Components::Camera& currentCam = cameraMap.at(m_currentCamera);
		currentCam.SetPosition(pos);
	}
}

