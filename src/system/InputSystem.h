#pragma once
#include "SDL3/SDL.h"
#include <unordered_map>
#include <string>
#include "Components.h"
namespace System
{
	class InputSystem
	{
	public:
		InputSystem() = default;
		~InputSystem() = default;
		void HandleInput(SDL_Event& event);
		void RegisterAction(Components::InputAction action);
		bool IsActionPressed(const std::string& actionName) const;
		bool IsWindowCloseRequested() const
		{
			return m_isWindowCloseRequested;
		}
		const Vec2& GetMousePosition(const std::string& actionName) const
		{
			return m_registeredActions.at(actionName).mousePosition;
		}
		const Vec2& GetMouseDelta(const std::string& actionName) const
		{
			return m_registeredActions.at(actionName).mouseDelta;
		}


	private:
		void HandleKeyDown(SDL_Event& event);
		void HandleKeyUp(SDL_Event& event);
		void HandleMouseButtonDown(SDL_Event& event);
		void HandleMouseButtonUp(SDL_Event& event);
		void HandleMouseMotion(SDL_Event& event);
		void HandleWindowCloseRequest(SDL_Event& event);
		std::unordered_map<std::string, Components::InputAction> m_registeredActions;
		bool m_isWindowCloseRequested = false;
	};
}
